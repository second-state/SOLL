// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
object "Contract1" {
    code {
        let A := datasize("Contract2")
        let B := datasize("Contract1.Contract2")
        let C := datasize("Contract2.runtime")
        let D := datasize("Contract2.runtime.Table2")
        let E := datasize("Contract1.Contract2.runtime.Table2")
        let F := datasize("Contract1")
    }
    object "Contract2" {
        code {
            // code here ...
        }

        object "runtime" {
            code {
                // code here ...
            }
            data "Table2" hex"4123"
        }

        data "Table1" hex"4123"
    }
}