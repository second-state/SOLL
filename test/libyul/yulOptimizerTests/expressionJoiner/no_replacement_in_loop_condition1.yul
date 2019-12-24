// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
	for { let b := mload(1) } b {} {}
}
// ====
// step: expressionJoiner
// ----
// {
//     for { let b := mload(1) } b { }
//     { }
// }
