// RUN: %soll -lang=Yul %s
// REQUIRES: calldatasize
// REQUIRES: revert
// REQUIRES: mstore
{
	for {} calldatasize() { mstore(1, 2) } {
		let x := 7
		mstore(4, 5)
		revert(0, x)
	}
}
// ====
// step: controlFlowSimplifier
// ----
// {
//     if calldatasize()
//     {
//         let x := 7
//         mstore(4, 5)
//         revert(0, x)
//     }
// }
