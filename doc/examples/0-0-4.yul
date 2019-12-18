object "fib" {
  code {
    let f := 1
    let s := 1
    let next
    for { let i := 0 } ltu256(i, 10) { i := addu256(i, 1)}
    {
      if ltu256(i, 2) {
        sstore(i, 1)
      }
      if gtu256(i, 1) {
        next := addu256(s, f)
        f := s
        s := next
        sstore(i, s)
      }
    }
  }
  // Unreferenced data is not added to the assembled bytecode.
  data "str" "YUL Fibonacci"
}
