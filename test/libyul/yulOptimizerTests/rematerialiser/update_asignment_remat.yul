// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
// We cannot substitute `a` in `let b := a`
{
	let a := extcodesize(0)
	a := mul(a, 2)
	let b := a
}
// ====
// step: rematerialiser
// ----
// {
//     let a := extcodesize(0)
//     a := mul(a, 2)
//     let b := a
// }
