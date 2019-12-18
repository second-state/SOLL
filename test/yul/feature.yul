// RUN: %soll --lang=Yul %s
object "feature" {
  code {
    function store(key, value) -> result {
      sstore(key, value)
    }

    let i := 0
    let j := 1
    switch 3
    case 0 {
      break
      sstore(i, j)

    }
    default {
      break
    }

  }
  // Unreferenced data is not added to the assembled bytecode.
  data "str" "CodeGen is under implementation."
}
