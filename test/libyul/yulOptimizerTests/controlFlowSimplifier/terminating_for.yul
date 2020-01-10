// RUN: %soll -lang=Yul %s
// REQUIRES: calldatasize
// REQUIRES: mstore
{
	for {} calldatasize() { mstore(1, 2) } {
		mstore(4, 5)
		break
	}
}
// ====
// step: controlFlowSimplifier
// ----
// {
//     if calldatasize() { mstore(4, 5) }
// }
