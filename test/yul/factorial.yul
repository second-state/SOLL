object "factorial" {
  code {
    let fact := 1
    for { let i := 1 } ltu256(i, 10) { i := addu256(i, 1)}
    {
      fact := mulu256(fact, i)
    }
    sstore(0, fact)
  }
  // Unreferenced data is not added to the assembled bytecode.
  data "str" "Yul Factorial"
}
