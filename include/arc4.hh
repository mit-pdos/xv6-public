class arc4 {
 public:
  arc4(const u8 *key, size_t nbytes) {
    reset();
    for (size_t n = 0; n < nbytes; n += 128)
      addkey(&key[n], nbytes - n > 128 ? 128 : nbytes - n);
    j = i;
  }

  u8 getbyte() {
    uint8_t si, sj;
    i = (i + 1) & 0xff;
    si = s[i];
    j = (j + si) & 0xff;
    sj = s[j];
    s[i] = sj;
    s[j] = si;
    return s[(si + sj) & 0xff];
  }

  template<class T> T rand() {
    T v;
    for (u32 i = 0; i < sizeof(v); i++)
      i[(u8*) &v] = getbyte();
    return v;
  }

 private:
  void reset() {
    i = 0xff;
    j = 0;
    for (u32 n = 0; n < 0x100; n++)
      s[n] = n;
  }

  void addkey(const u8 *key, size_t nbytes) {
    size_t n, keypos;
    uint8_t si;
    for (n = 0, keypos = 0; n < 256; n++, keypos++) {
      if (keypos >= nbytes)
        keypos = 0;
      i = (i + 1) & 0xff;
      si = s[i];
      j = (j + si + key[keypos]) & 0xff;
      s[i] = s[j];
      s[j] = si;
    }
  }

  u8 i;
  u8 j;
  u8 s[256];
};
