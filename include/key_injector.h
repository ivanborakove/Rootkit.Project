#ifndef KEY_INJECTOR_H
#define KEY_INJECTOR_H

int setup_key_injector(void);
void cleanup_key_injector(void);
void inject_key(unsigned int keycode);

#endif
