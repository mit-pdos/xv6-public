#if CILKENABLE

template<typename A1>
static void
cilk_call(void (*fn)(A1), A1 a1)
{
  cilk_push((void(*)(uptr, uptr))fn, (uptr)a1, (uptr)0);
}

template<typename A1, typename A2>
static void
cilk_call(void (*fn)(A1, A2), A1 a1, A2 a2)
{
  cilk_push((void(*)(uptr, uptr))fn, (uptr)a1, (uptr)a2);
}

#else // !CILKENABLE

template<typename A1>
static void
cilk_call(void (*fn)(A1), A1 a1)
{
  fn(a1);
}

template<typename A1, typename A2>
static void
cilk_call(void (*fn)(A1, A2), A1 a1, A2 a2)
{
  fn(a1, a2);
}
#endif
