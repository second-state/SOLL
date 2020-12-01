// RUN: %soll --lang=Yul %s
object "runtime" {
        code {
            function read_bar(key) -> return_val
            {
                {
                    return_val := scopy(dualkeccak256(0, key), 10)
                    leave
                }
                free(add(return_val, 10))
            }
            function write_bar(key, value)
            {
                let ptr := avail()
                mcopy(value, dualkeccak256(0, key), 10)
                free(ptr)
            }
            function avail() -> ptr
            { ptr := mload(0x00) }
            function alloc(size) -> ptr
            {
                ptr := mload(0x00)
                if eq(ptr, 0x00) { ptr := 0x20 }
                mstore(0x00, add(ptr, size))
            }
            function alloc_mstoren(val, size) -> ptr
            {
                ptr := alloc(size)
                mstoren(ptr, val, size)
            }
            function free(ptr)
            { mstore(0x00, ptr) }
            function ccopy(cptr, size) -> mptr
            {
                mptr := alloc(size)
                calldatacopy(mptr, cptr, size)
            }
            function mcopy(mptr, sptr, size)
            {
                for { let i := 0 } lt(i, size) { i := add(i, 1) }
                {
                    let _mptr := add(mptr, i)
                    let _sptr := add(sptr, i)
                    sstoren(_sptr, mloadn(_mptr, 1), 1)
                }
            }
            function scopy(sptr, size) -> mptr
            {
                mptr := alloc(size)
                for { let i := 0 } lt(i, size) { i := add(i, 1) }
                {
                    let _mptr := add(mptr, i)
                    let _sptr := add(sptr, i)
                    mstoren(_mptr, sloadn(_sptr, 1), 1)
                }
            }
            function mloadn(ptr, size) -> val
            {
                val := shr(sub(256, mul(8, size)), mload(ptr))
            }
            function sloadn(ptr, size) -> val
            {
                val := shr(sub(256, mul(8, size)), sload(ptr))
            }
            function cloadn(ptr, size) -> val
            {
                val := shr(sub(256, mul(8, size)), calldataload(ptr))
            }
            function mstoren(ptr, val, size)
            {
                let size_bits := mul(8, size)
                let left := shl(sub(256, size_bits), val)
                let right := shr(size_bits, mload(add(ptr, size)))
                mstore(ptr, or(left, right))
            }
            function sstoren(ptr, val, size)
            {
                let size_bits := mul(8, size)
                let left := shl(sub(256, size_bits), val)
                let right := shr(size_bits, sload(add(ptr, size)))
                sstore(ptr, or(left, right))
            }
            function dualkeccak256(a, b) -> return_val
            {
                let ptr := avail()
                mstore(ptr, a)
                mstore(add(ptr, 32), b)
                return_val := keccak256(ptr, 64)
            }
            switch cloadn(0, 4)
            case 0xd8d749a4 {
                return(read_bar(calldataload(4)), 32)
            }
            case 0x281388a3 {
                write_bar(calldataload(4), ccopy(36, 10))
            }
        }
    }

