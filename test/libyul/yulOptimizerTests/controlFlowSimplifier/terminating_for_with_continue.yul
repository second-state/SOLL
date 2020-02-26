// RUN: %soll -lang=Yul %s
// REQUIRES: calldatasize
// REQUIRES: mstore
{
	for {} calldatasize() { mstore(1, 2) } {
		if calldatasize() { continue }
		mstore(4, 5)
		break
	}
}
// ====
// step: controlFlowSimplifier
// ----
// {
//     for { } calldatasize() { mstore(1, 2) }
//     {
//         if calldatasize() { continue }
//         mstore(4, 5)
//         break
//     }
// }
