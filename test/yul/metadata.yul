// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
object "metadata_x" {
	code {
		// sstore(dataoffset(".metadata.x"), 0)
        let size := datasize("x")
	}

	object ".metadata" {
		code {}
		data "x" "ABC"
	}

    data "x" "CDE"
}
