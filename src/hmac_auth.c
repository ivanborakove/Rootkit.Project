#include "hmac_auth.h"
#include <linux/crypto.h>
#include <crypto/hash.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/string.h>

bool validate_hmac(const unsigned char *data, size_t data_len, const unsigned char *received_hmac) {
    struct crypto_shash *tfm;
    struct shash_desc *desc;
    unsigned char result[HMAC_DIGEST_SIZE];
    size_t size;
    int ret;
    bool match = false;

    tfm = crypto_alloc_shash("hmac(sha256)", 0, 0);
    if (IS_ERR(tfm))
        return false;

    ret = crypto_shash_setkey(tfm, HMAC_KEY, HMAC_KEY_LEN);
    if (ret)
        goto out_free_tfm;

    size = sizeof(*desc) + crypto_shash_descsize(tfm);
    desc = kzalloc(size, GFP_KERNEL);
    if (!desc)
        goto out_free_tfm;

    desc->tfm = tfm;

    ret = crypto_shash_init(desc);
    if (ret) goto out_free_desc;

    ret = crypto_shash_update(desc, data, data_len);
    if (ret) goto out_free_desc;

    ret = crypto_shash_final(desc, result);
    if (ret) goto out_free_desc;

    match = (memcmp(result, received_hmac, HMAC_DIGEST_SIZE) == 0);

out_free_desc:
    kfree(desc);
out_free_tfm:
    crypto_free_shash(tfm);
    return match;
}

void compute_hmac(const char *key, const unsigned char *data, size_t len, unsigned char *output) {
    struct crypto_shash *tfm;
    struct shash_desc *desc;
    size_t size;
    int ret;

    tfm = crypto_alloc_shash("hmac(sha256)", 0, 0);
    if (IS_ERR(tfm))
        return;

    ret = crypto_shash_setkey(tfm, key, strlen(key));
    if (ret)
        goto out_free_tfm;

    size = sizeof(*desc) + crypto_shash_descsize(tfm);
    desc = kzalloc(size, GFP_KERNEL);
    if (!desc)
        goto out_free_tfm;

    desc->tfm = tfm;

    if (crypto_shash_init(desc))
        goto out_free_desc;

    if (crypto_shash_update(desc, data, len))
        goto out_free_desc;

    crypto_shash_final(desc, output);

out_free_desc:
    kfree(desc);
out_free_tfm:
    crypto_free_shash(tfm);
}
