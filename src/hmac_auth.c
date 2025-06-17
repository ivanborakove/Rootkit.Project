#include "hmac_auth.h"
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>

bool validate_hmac(const unsigned char *data, size_t data_len, const unsigned char *received_hmac) {
    struct crypto_shash *tfm;
    struct shash_desc *shash;
    unsigned char result[HMAC_DIGEST_SIZE];
    int ret;

    tfm = crypto_alloc_shash("hmac(sha256)", 0, 0);
    if (IS_ERR(tfm))
        return false;

    ret = crypto_shash_setkey(tfm, HMAC_KEY, HMAC_KEY_LEN);
    if (ret) {
        crypto_free_shash(tfm);
        return false;
    }

    shash = kmalloc(sizeof(*shash) + crypto_shash_descsize(tfm), GFP_KERNEL);
    if (!shash) {
        crypto_free_shash(tfm);
        return false;
    }

    shash->tfm = tfm;
    shash->flags = 0x0;

    ret = crypto_shash_init(shash);
    if (ret) goto out;

    ret = crypto_shash_update(shash, data, data_len);
    if (ret) goto out;

    ret = crypto_shash_final(shash, result);
    if (ret) goto out;

    crypto_free_shash(tfm);
    kfree(shash);

    if (memcmp(result, received_hmac, HMAC_DIGEST_SIZE) == 0)
        return true;

    return false;

out:
    crypto_free_shash(tfm);
    kfree(shash);
    return false;
}
