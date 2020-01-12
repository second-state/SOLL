// RUN: %soll -lang=Yul %s
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
