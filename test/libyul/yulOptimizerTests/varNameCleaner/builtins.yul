// RUN: %soll -lang=Yul %s
{
	let datasize_256 := 1
}
// ====
// step: varNameCleaner
// ----
// { let datasize_1 := 1 }
