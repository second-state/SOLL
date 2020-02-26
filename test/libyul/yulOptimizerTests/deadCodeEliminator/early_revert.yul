// RUN: %soll -lang=Yul %s
// REQUIRES: mstore
// REQUIRES: revert
{
    let b := 20
    revert(0, 0)
    for {
        let a := 20
    }
    lt(a, 40)
    {
        a := add(a, 2)
    }
    {
        a := a
        mstore(0, a)
        a := add(a, 10)
    }
}

// ====
// step: deadCodeEliminator
// ----
// {
//     let b := 20
//     revert(0, 0)
// }
