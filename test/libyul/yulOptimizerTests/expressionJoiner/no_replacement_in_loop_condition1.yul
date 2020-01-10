// RUN: %soll -lang=Yul %s
// REQUIRES: mload
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
