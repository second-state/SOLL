// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
	let a := mload(0)
	for { } a {} {}
}
// ====
// step: expressionJoiner
// ----
// {
//     let a := mload(0)
//     for { } a { }
//     { }
// }
