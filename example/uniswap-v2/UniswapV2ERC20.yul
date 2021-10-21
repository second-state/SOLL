object "UniswapV2ERC20_431_deployed" {
    code {
        /// @src 0:104,3813
        mstore(64, 128)

        if iszero(lt(calldatasize(), 4))
        {
            let selector := shift_right_224_unsigned(calldataload(0))
            switch selector

            case 0x06fdde03
            {
                // name()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_name_12()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_string_memory_ptr__to_t_string_memory_ptr__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x095ea7b3
            {
                // approve(address,uint256)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_uint256(4, calldatasize())
                let ret_0 :=  fun_approve_275(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x18160ddd
            {
                // totalSupply()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_totalSupply_23()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x23b872dd
            {
                // transferFrom(address,address,uint256)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1, param_2 :=  abi_decode_tuple_t_addresst_addresst_uint256(4, calldatasize())
                let ret_0 :=  fun_transferFrom_350(param_0, param_1, param_2)
                sstore(0x2, param_0)
                sstore(0x3, param_1)
                sstore(0x4, param_2)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x30adf81f
            {
                // PERMIT_TYPEHASH()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_PERMIT_TYPEHASH_42()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bytes32__to_t_bytes32__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x313ce567
            {
                // decimals()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_decimals_20()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint8__to_t_uint8__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x3644e515
            {
                // DOMAIN_SEPARATOR()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_DOMAIN_SEPARATOR_38()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bytes32__to_t_bytes32__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x40c10f19
            {
                // mint(address,uint256)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_uint256(4, calldatasize())
                let ret_0 :=  fun_mint_123(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x70a08231
            {
                // balanceOf(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                let ret_0 :=  getter_fun_balanceOf_28(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x7ecebe00
            {
                // nonces(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                let ret_0 :=  getter_fun_nonces_47(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x95d89b41
            {
                // symbol()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_symbol_16()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_string_memory_ptr__to_t_string_memory_ptr__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xa9059cbb
            {
                // transfer(address,uint256)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_uint256(4, calldatasize())
                let ret_0 :=  fun_transfer_295(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xd505accf
            {
                // permit(address,address,uint256,uint256,uint8,bytes32,bytes32)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1, param_2, param_3, param_4, param_5, param_6 :=  abi_decode_tuple_t_addresst_addresst_uint256t_uint256t_uint8t_bytes32t_bytes32(4, calldatasize())
                fun_permit_430(param_0, param_1, param_2, param_3, param_4, param_5, param_6)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                return(memPos, sub(memEnd, memPos))
            }

            case 0xdd62ed3e
            {
                // allowance(address,address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_address(4, calldatasize())
                let ret_0 :=  getter_fun_allowance_35(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            default {}
        }
        if iszero(calldatasize()) {  }

        function abi_decode_t_address(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_address(value)
        }

        function abi_decode_t_bytes32(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_bytes32(value)
        }

        function abi_decode_t_uint256(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_uint256(value)
        }

        function abi_decode_t_uint8(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_uint8(value)
        }

        function abi_decode_tuple_(headStart, dataEnd)   {
            if slt(sub(dataEnd, headStart), 0) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

        }

        function abi_decode_tuple_t_address(headStart, dataEnd) -> value0 {
            if slt(sub(dataEnd, headStart), 32) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

        }

        function abi_decode_tuple_t_addresst_address(headStart, dataEnd) -> value0, value1 {
            if slt(sub(dataEnd, headStart), 64) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

            {

                let offset := 32

                value1 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

        }

        function abi_decode_tuple_t_addresst_addresst_uint256(headStart, dataEnd) -> value0, value1, value2 {
            if slt(sub(dataEnd, headStart), 96) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

            {

                let offset := 32

                value1 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

            {

                let offset := 64

                value2 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
            }

        }

        function abi_decode_tuple_t_addresst_addresst_uint256t_uint256t_uint8t_bytes32t_bytes32(headStart, dataEnd) -> value0, value1, value2, value3, value4, value5, value6 {
            if slt(sub(dataEnd, headStart), 224) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

            {

                let offset := 32

                value1 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

            {

                let offset := 64

                value2 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
            }

            {

                let offset := 96

                value3 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
            }

            {

                let offset := 128

                value4 := abi_decode_t_uint8(add(headStart, offset), dataEnd)
            }

            {

                let offset := 160

                value5 := abi_decode_t_bytes32(add(headStart, offset), dataEnd)
            }

            {

                let offset := 192

                value6 := abi_decode_t_bytes32(add(headStart, offset), dataEnd)
            }

        }

        function abi_decode_tuple_t_addresst_uint256(headStart, dataEnd) -> value0, value1 {
            if slt(sub(dataEnd, headStart), 64) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

            {

                let offset := 32

                value1 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
            }

        }

        function abi_encode_t_address_to_t_address_fromStack(value, pos) {
            mstore(pos, cleanup_t_address(value))
        }

        function abi_encode_t_bool_to_t_bool_fromStack(value, pos) {
            mstore(pos, cleanup_t_bool(value))
        }

        function abi_encode_t_bytes32_to_t_bytes32_fromStack(value, pos) {
            mstore(pos, cleanup_t_bytes32(value))
        }

        function abi_encode_t_bytes32_to_t_bytes32_nonPadded_inplace_fromStack(value, pos) {
            mstore(pos, leftAlign_t_bytes32(cleanup_t_bytes32(value)))
        }

        function abi_encode_t_string_memory_ptr_to_t_string_memory_ptr_fromStack(value, pos) -> end {
            let length := array_length_t_string_memory_ptr(value)
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, length)
            copy_memory_to_memory(add(value, 0x20), pos, length)
            end := add(pos, round_up_to_mul_of_32(length))
        }

        function abi_encode_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 21)
            store_literal_in_memory_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 28)
            store_literal_in_memory_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_301a50b291d33ce1e8e9064e3f6a6c51d902ec22892b50d58abf6357c6a45541_to_t_string_memory_ptr_nonPadded_inplace_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_nonPadded_inplace_fromStack(pos, 2)
            store_literal_in_memory_301a50b291d33ce1e8e9064e3f6a6c51d902ec22892b50d58abf6357c6a45541(pos)
            end := add(pos, 2)
        }

        function abi_encode_t_stringliteral_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 20)
            store_literal_in_memory_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 18)
            store_literal_in_memory_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_uint256_to_t_uint256_fromStack(value, pos) {
            mstore(pos, cleanup_t_uint256(value))
        }

        function abi_encode_t_uint8_to_t_uint8_fromStack(value, pos) {
            mstore(pos, cleanup_t_uint8(value))
        }

        function abi_encode_tuple__to__fromStack(headStart ) -> tail {
            tail := add(headStart, 0)

        }

        function abi_encode_tuple_packed_t_stringliteral_301a50b291d33ce1e8e9064e3f6a6c51d902ec22892b50d58abf6357c6a45541_t_bytes32_t_bytes32__to_t_string_memory_ptr_t_bytes32_t_bytes32__nonPadded_inplace_fromStack(pos , value0, value1) -> end {

            pos := abi_encode_t_stringliteral_301a50b291d33ce1e8e9064e3f6a6c51d902ec22892b50d58abf6357c6a45541_to_t_string_memory_ptr_nonPadded_inplace_fromStack( pos)

            abi_encode_t_bytes32_to_t_bytes32_nonPadded_inplace_fromStack(value0,  pos)
            pos := add(pos, 32)

            abi_encode_t_bytes32_to_t_bytes32_nonPadded_inplace_fromStack(value1,  pos)
            pos := add(pos, 32)

            end := pos
        }

        function abi_encode_tuple_t_bool__to_t_bool__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_bool_to_t_bool_fromStack(value0,  add(headStart, 0))

        }

        function abi_encode_tuple_t_bytes32__to_t_bytes32__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_bytes32_to_t_bytes32_fromStack(value0,  add(headStart, 0))

        }

        function abi_encode_tuple_t_bytes32_t_address_t_address_t_uint256_t_uint256_t_uint256__to_t_bytes32_t_address_t_address_t_uint256_t_uint256_t_uint256__fromStack(headStart , value0, value1, value2, value3, value4, value5) -> tail {
            tail := add(headStart, 192)

            abi_encode_t_bytes32_to_t_bytes32_fromStack(value0,  add(headStart, 0))

            abi_encode_t_address_to_t_address_fromStack(value1,  add(headStart, 32))

            abi_encode_t_address_to_t_address_fromStack(value2,  add(headStart, 64))

            abi_encode_t_uint256_to_t_uint256_fromStack(value3,  add(headStart, 96))

            abi_encode_t_uint256_to_t_uint256_fromStack(value4,  add(headStart, 128))

            abi_encode_t_uint256_to_t_uint256_fromStack(value5,  add(headStart, 160))

        }

        function abi_encode_tuple_t_bytes32_t_uint8_t_bytes32_t_bytes32__to_t_bytes32_t_uint8_t_bytes32_t_bytes32__fromStack(headStart , value0, value1, value2, value3) -> tail {
            tail := add(headStart, 128)

            abi_encode_t_bytes32_to_t_bytes32_fromStack(value0,  add(headStart, 0))

            abi_encode_t_uint8_to_t_uint8_fromStack(value1,  add(headStart, 32))

            abi_encode_t_bytes32_to_t_bytes32_fromStack(value2,  add(headStart, 64))

            abi_encode_t_bytes32_to_t_bytes32_fromStack(value3,  add(headStart, 96))

        }

        function abi_encode_tuple_t_string_memory_ptr__to_t_string_memory_ptr__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_string_memory_ptr_to_t_string_memory_ptr_fromStack(value0,  tail)

        }

        function abi_encode_tuple_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_uint256__to_t_uint256__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

        }

        function abi_encode_tuple_t_uint8__to_t_uint8__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_uint8_to_t_uint8_fromStack(value0,  add(headStart, 0))

        }

        function allocate_memory(size) -> memPtr {
            memPtr := allocate_unbounded()
            finalize_allocation(memPtr, size)
        }

        function allocate_memory_array_t_string_memory_ptr(length) -> memPtr {
            let allocSize := array_allocation_size_t_string_memory_ptr(length)
            memPtr := allocate_memory(allocSize)

            mstore(memPtr, length)

        }

        function allocate_unbounded() -> memPtr {
            memPtr := mload(64)
        }

        function array_allocation_size_t_string_memory_ptr(length) -> size {
            // Make sure we can allocate memory without overflow
            if gt(length, 0xffffffffffffffff) { panic_error_0x41() }

            size := round_up_to_mul_of_32(length)

            // add length slot
            size := add(size, 0x20)

        }

        function array_dataslot_t_bytes_memory_ptr(ptr) -> data {
            data := ptr

            data := add(ptr, 0x20)

        }

        function array_length_t_bytes_memory_ptr(value) -> length {

            length := mload(value)

        }

        function array_length_t_string_memory_ptr(value) -> length {

            length := mload(value)

        }

        function array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, length) -> updated_pos {
            mstore(pos, length)
            updated_pos := add(pos, 0x20)
        }

        function array_storeLengthForEncoding_t_string_memory_ptr_nonPadded_inplace_fromStack(pos, length) -> updated_pos {
            updated_pos := pos
        }

        function checked_add_t_uint256(x, y) -> sum {
            x := cleanup_t_uint256(x)
            y := cleanup_t_uint256(y)

            // overflow, if x > (maxValue - y)
            if gt(x, sub(0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff, y)) { panic_error_0x11() }

            sum := add(x, y)
        }

        function checked_sub_t_uint256(x, y) -> diff {
            x := cleanup_t_uint256(x)
            y := cleanup_t_uint256(y)

            if lt(x, y) { panic_error_0x11() }

            diff := sub(x, y)
        }

        function cleanup_from_storage_t_bytes32(value) -> cleaned {
            cleaned := value
        }

        function cleanup_from_storage_t_uint256(value) -> cleaned {
            cleaned := value
        }

        function cleanup_t_address(value) -> cleaned {
            cleaned := cleanup_t_uint160(value)
        }

        function cleanup_t_bool(value) -> cleaned {
            cleaned := iszero(iszero(value))
        }

        function cleanup_t_bytes32(value) -> cleaned {
            cleaned := value
        }

        function cleanup_t_int256(value) -> cleaned {
            cleaned := value
        }

        function cleanup_t_rational_49955707469362902507454157297736832118868343942642399513960811609542965143241_by_1(value) -> cleaned {
            cleaned := value
        }

        function cleanup_t_uint160(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffff)
        }

        function cleanup_t_uint256(value) -> cleaned {
            cleaned := value
        }

        function cleanup_t_uint8(value) -> cleaned {
            cleaned := and(value, 0xff)
        }

        /// @src 0:666,783
        function constant_PERMIT_TYPEHASH_42() -> ret {
            /// @src 0:717,783
            let expr_41 := 0x6e71edae12b1b97f4d1f60370fef10105fa2faae0126114a169c64845d6126c9
            let _2 := convert_t_rational_49955707469362902507454157297736832118868343942642399513960811609542965143241_by_1_to_t_bytes32(expr_41)

            ret := _2
        }

        /// @src 0:295,339
        function constant_decimals_20() -> ret {
            /// @src 0:337,339
            let expr_19 := 0x12
            let _3 := convert_t_rational_18_by_1_to_t_uint8(expr_19)

            ret := _3
        }

        /// @src 0:183,234
        function constant_name_12() -> ret_mpos {
            /// @src 0:222,234
            let _1_mpos := convert_t_stringliteral_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738_to_t_string_memory_ptr()

            ret_mpos := _1_mpos
        }

        /// @src 0:240,289
        function constant_symbol_16() -> ret_mpos {
            /// @src 0:281,289
            let _4_mpos := convert_t_stringliteral_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255_to_t_string_memory_ptr()

            ret_mpos := _4_mpos
        }

        function convert_t_address_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_address(value)
        }

        function convert_t_int256_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_0_by_1_to_t_address(value) -> converted {
            converted := convert_t_rational_0_by_1_to_t_uint160(value)
        }

        function convert_t_rational_0_by_1_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_rational_18_by_1_to_t_uint8(value) -> converted {
            converted := cleanup_t_uint8(value)
        }

        function convert_t_rational_49955707469362902507454157297736832118868343942642399513960811609542965143241_by_1_to_t_bytes32(value) -> converted {
            converted := shift_left_0(cleanup_t_rational_49955707469362902507454157297736832118868343942642399513960811609542965143241_by_1(value))
        }

        function convert_t_rational_minus_1_by_1_to_t_int256(value) -> converted {
            converted := cleanup_t_int256(value)
        }

        function convert_t_stringliteral_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255_to_t_string_memory_ptr() -> converted {
            converted := copy_literal_to_memory_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255()
        }

        function convert_t_stringliteral_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738_to_t_string_memory_ptr() -> converted {
            converted := copy_literal_to_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738()
        }

        function convert_t_uint160_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_uint160(value)
        }

        function convert_t_uint160_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_uint256_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function copy_literal_to_memory_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255() -> memPtr {
            memPtr := allocate_memory_array_t_string_memory_ptr(6)
            store_literal_in_memory_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255(add(memPtr, 32))
        }

        function copy_literal_to_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738() -> memPtr {
            memPtr := allocate_memory_array_t_string_memory_ptr(10)
            store_literal_in_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738(add(memPtr, 32))
        }

        function copy_memory_to_memory(src, dst, length) {
            let i := 0
            for { } lt(i, length) { i := add(i, 32) }
            {
                mstore(add(dst, i), mload(add(src, i)))
            }
            if gt(i, length)
            {
                // clear end
                mstore(add(dst, length), 0)
            }
        }

        function extract_from_storage_value_dynamict_bytes32(slot_value, offset) -> value {
            value := cleanup_from_storage_t_bytes32(shift_right_unsigned_dynamic(mul(offset, 8), slot_value))
        }

        function extract_from_storage_value_dynamict_uint256(slot_value, offset) -> value {
            value := cleanup_from_storage_t_uint256(shift_right_unsigned_dynamic(mul(offset, 8), slot_value))
        }

        function extract_from_storage_value_offset_0t_bytes32(slot_value) -> value {
            value := cleanup_from_storage_t_bytes32(shift_right_0_unsigned(slot_value))
        }

        function extract_from_storage_value_offset_0t_uint256(slot_value) -> value {
            value := cleanup_from_storage_t_uint256(shift_right_0_unsigned(slot_value))
        }

        function finalize_allocation(memPtr, size) {
            let newFreePtr := add(memPtr, round_up_to_mul_of_32(size))
            // protect against overflow
            if or(gt(newFreePtr, 0xffffffffffffffff), lt(newFreePtr, memPtr)) { panic_error_0x41() }
            mstore(64, newFreePtr)
        }

        function fun__approve_217(var_owner_195, var_spender_197, var_value_199) {
            /// @src 0:2121,2287

            /// @src 0:2229,2234
            let _24 := var_value_199
            let expr_207 := _24
            /// @src 0:2201,2210
            let _25 := 0x02
            let expr_202 := _25
            /// @src 0:2211,2216
            let _26 := var_owner_195
            let expr_203 := _26
            /// @src 0:2201,2217
            let _27 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_202,expr_203)
            let _28 := _27
            let expr_205 := _28
            /// @src 0:2218,2225
            let _29 := var_spender_197
            let expr_204 := _29
            /// @src 0:2201,2226
            let _30 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_205,expr_204)
            /// @src 0:2201,2234
            update_storage_value_offset_0t_uint256_to_t_uint256(_30, expr_207)
            let expr_208 := expr_207
            /// @src 0:2258,2263
            let _31 := var_owner_195
            let expr_211 := _31
            /// @src 0:2265,2272
            let _32 := var_spender_197
            let expr_212 := _32
            /// @src 0:2274,2279
            let _33 := var_value_199
            let expr_213 := _33
            /// @src 0:2249,2280
            let _34 := 0x8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925
            {
                let _35 := allocate_unbounded()
                let _36 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_35 , expr_213)
                log3(_35, sub(_36, _35) , _34, expr_211, expr_212)
            }
        }

        function fun__transfer_255(var_from_219, var_to_221, var_value_223) {
            /// @src 0:2293,2509

            /// @src 0:2386,2395
            let _40 := 0x01
            let expr_229 := _40
            /// @src 0:2396,2400
            let _41 := var_from_219
            let expr_230 := _41
            /// @src 0:2386,2401
            let _42 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_229,expr_230)
            let _43 := read_from_storage_split_offset_0_t_uint256(_42)
            let expr_231 := _43
            /// @src 0:2386,2405
            let expr_232_self := expr_231
            let expr_232_functionIdentifier := 604
            /// @src 0:2406,2411
            let _44 := var_value_223
            let expr_233 := _44
            /// @src 0:2386,2412
            let expr_234 := fun_sub_604(expr_232_self, expr_233)
            /// @src 0:2368,2377
            let _45 := 0x01
            let expr_226 := _45
            /// @src 0:2378,2382
            let _46 := var_from_219
            let expr_227 := _46
            /// @src 0:2368,2383
            let _47 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_226,expr_227)
            /// @src 0:2368,2412
            // invalid()
            update_storage_value_offset_0t_uint256_to_t_uint256(_47, expr_234)
            let expr_235 := expr_234
            /// @src 0:2438,2447
            let _48 := 0x01
            let expr_240 := _48
            /// @src 0:2448,2450
            let _49 := var_to_221
            let expr_241 := _49
            /// @src 0:2438,2451
            let _50 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_240,expr_241)
            let _51 := read_from_storage_split_offset_0_t_uint256(_50)
            let expr_242 := _51
            /// @src 0:2438,2455
            let expr_243_self := expr_242
            let expr_243_functionIdentifier := 582
            /// @src 0:2456,2461
            let _52 := var_value_223
            let expr_244 := _52
            /// @src 0:2438,2462
            let expr_245 := fun_add_582(expr_243_self, expr_244)
            /// @src 0:2422,2431
            let _53 := 0x01
            let expr_237 := _53
            /// @src 0:2432,2434
            let _54 := var_to_221
            let expr_238 := _54
            /// @src 0:2422,2435
            let _55 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_237,expr_238)
            /// @src 0:2422,2462
            update_storage_value_offset_0t_uint256_to_t_uint256(_55, expr_245)
            let expr_246 := expr_245
            /// @src 0:2486,2490
            let _56 := var_from_219
            let expr_249 := _56
            /// @src 0:2492,2494
            let _57 := var_to_221
            let expr_250 := _57
            /// @src 0:2496,2501
            let _58 := var_value_223
            let expr_251 := _58
            /// @src 0:2477,2502
            let _59 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
            {
                let _60 := allocate_unbounded()
                let _61 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_60 , expr_251)
                log3(_60, sub(_61, _60) , _59, expr_249, expr_250)
            }
        }

        function fun_add_582(var_x_562, var_y_564) -> var_z_567 {
            /// @src 2:154,280
            /// @src 2:206,212
            let zero_t_uint256_112 := zero_value_for_split_t_uint256()
            var_z_567 := zero_t_uint256_112

            /// @src 2:237,238
            let _113 := var_x_562
            let expr_571 := _113
            /// @src 2:241,242
            let _114 := var_y_564
            let expr_572 := _114
            /// @src 2:237,242
            let expr_573 := checked_add_t_uint256(expr_571, expr_572)

            /// @src 2:233,242
            var_z_567 := expr_573
            let expr_574 := expr_573
            /// @src 2:232,243
            let expr_575 := expr_574
            /// @src 2:247,248
            let _115 := var_x_562
            let expr_576 := _115
            /// @src 2:232,248
            let expr_577 := iszero(lt(cleanup_t_uint256(expr_575), cleanup_t_uint256(expr_576)))
            /// @src 2:224,273
            require_helper_t_stringliteral_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db(expr_577)

        }

        function fun_approve_275(var_spender_257, var_value_259) -> var__263 {
            /// @src 0:2515,2668
            /// @src 0:2588,2592
            let zero_t_bool_21 := zero_value_for_split_t_bool()
            var__263 := zero_t_bool_21

            /// @src 0:2604,2612
            let expr_265_functionIdentifier := 217
            /// @src 0:2613,2623
            let expr_267 := caller()
            /// @src 0:2625,2632
            let _22 := var_spender_257
            let expr_268 := _22
            /// @src 0:2634,2639
            let _23 := var_value_259
            let expr_269 := _23
            fun__approve_217(expr_267, expr_268, expr_269)
            /// @src 0:2657,2661
            let expr_272 := 0x01
            /// @src 0:2650,2661
            var__263 := expr_272
            leave

        }

        function fun_mint_123(var_to_85, var_value_87) -> var__91 {
            /// @src 0:1456,1697
            /// @src 0:1521,1525
            let zero_t_bool_5 := zero_value_for_split_t_bool()
            var__91 := zero_t_bool_5

            /// @src 0:1551,1562
            let _6 := read_from_storage_split_offset_0_t_uint256(0x00)
            let expr_94 := _6
            /// @src 0:1551,1566
            let expr_95_self := expr_94
            let expr_95_functionIdentifier := 582
            /// @src 0:1567,1572
            let _7 := var_value_87
            let expr_96 := _7
            /// @src 0:1551,1573
            let expr_97 := fun_add_582(expr_95_self, expr_96)
            /// @src 0:1537,1573
            update_storage_value_offset_0t_uint256_to_t_uint256(0x00, expr_97)
            let expr_98 := expr_97
            /// @src 0:1599,1608
            let _8 := 0x01
            let expr_103 := _8
            /// @src 0:1609,1611
            let _9 := var_to_85
            let expr_104 := _9
            /// @src 0:1599,1612
            let _10 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_103,expr_104)
            let _11 := read_from_storage_split_offset_0_t_uint256(_10)
            let expr_105 := _11
            /// @src 0:1599,1616
            let expr_106_self := expr_105
            let expr_106_functionIdentifier := 582
            /// @src 0:1617,1622
            let _12 := var_value_87
            let expr_107 := _12
            /// @src 0:1599,1623
            let expr_108 := fun_add_582(expr_106_self, expr_107)
            /// @src 0:1583,1592
            let _13 := 0x01
            let expr_100 := _13
            /// @src 0:1593,1595
            let _14 := var_to_85
            let expr_101 := _14
            /// @src 0:1583,1596
            let _15 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_100,expr_101)
            /// @src 0:1583,1623
            update_storage_value_offset_0t_uint256_to_t_uint256(_15, expr_108)
            let expr_109 := expr_108
            /// @src 0:1655,1656
            let expr_114 := 0x00
            /// @src 0:1647,1657
            let expr_115 := convert_t_rational_0_by_1_to_t_address(expr_114)
            /// @src 0:1659,1661
            let _16 := var_to_85
            let expr_116 := _16
            /// @src 0:1663,1668
            let _17 := var_value_87
            let expr_117 := _17
            /// @src 0:1638,1669
            let _18 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
            {
                let _19 := allocate_unbounded()
                let _20 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_19 , expr_117)
                log3(_19, sub(_20, _19) , _18, expr_115, expr_116)
            }/// @src 0:1686,1690
            let expr_120 := 0x01
            /// @src 0:1679,1690
            var__91 := expr_120
            leave

        }

        function fun_permit_430(var_owner_352, var_spender_354, var_value_356, var_deadline_358, var_v_360, var_r_362, var_s_364) {
            /// @src 0:3140,3811

            /// @src 0:3282,3290
            let _84 := var_deadline_358
            let expr_369 := _84
            /// @src 0:3294,3309
            let expr_371 := timestamp()
            /// @src 0:3282,3309
            let expr_372 := iszero(lt(cleanup_t_uint256(expr_369), cleanup_t_uint256(expr_371)))
            /// @src 0:3274,3332
            require_helper_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87(expr_372)
            /// @src 0:3444,3460
            let _85 := read_from_storage_split_offset_0_t_bytes32(0x03)
            let expr_382 := _85
            /// @src 0:3499,3514
            let expr_386 := constant_PERMIT_TYPEHASH_42()
            /// @src 0:3516,3521
            let _86 := var_owner_352
            let expr_387 := _86
            /// @src 0:3523,3530
            let _87 := var_spender_354
            let expr_388 := _87
            /// @src 0:3532,3537
            let _88 := var_value_356
            let expr_389 := _88
            /// @src 0:3539,3545
            let _89 := 0x04
            let expr_390 := _89
            /// @src 0:3546,3551
            let _90 := var_owner_352
            let expr_391 := _90
            /// @src 0:3539,3552
            let _91 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_390,expr_391)
            /// @src 0:3539,3554
            let _93 := read_from_storage_split_offset_0_t_uint256(_91)
            let _92 := increment_t_uint256(_93)
            update_storage_value_offset_0t_uint256_to_t_uint256(_91, _92)
            let expr_393 := _93
            /// @src 0:3556,3564
            let _94 := var_deadline_358
            let expr_394 := _94
            /// @src 0:3488,3565

            let expr_395_mpos := allocate_unbounded()
            let _95 := add(expr_395_mpos, 0x20)

            let _96 := abi_encode_tuple_t_bytes32_t_address_t_address_t_uint256_t_uint256_t_uint256__to_t_bytes32_t_address_t_address_t_uint256_t_uint256_t_uint256__fromStack(_95, expr_386, expr_387, expr_388, expr_389, expr_393, expr_394)
            mstore(expr_395_mpos, sub(_96, add(expr_395_mpos, 0x20)))
            finalize_allocation(expr_395_mpos, sub(_96, expr_395_mpos))
            /// @src 0:3478,3566
            let expr_396 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_395_mpos), array_length_t_bytes_memory_ptr(expr_395_mpos))
            /// @src 0:3382,3580

            let expr_397_mpos := allocate_unbounded()
            let _97 := add(expr_397_mpos, 0x20)

            let _98 := abi_encode_tuple_packed_t_stringliteral_301a50b291d33ce1e8e9064e3f6a6c51d902ec22892b50d58abf6357c6a45541_t_bytes32_t_bytes32__to_t_string_memory_ptr_t_bytes32_t_bytes32__nonPadded_inplace_fromStack(_97, expr_382, expr_396)
            mstore(expr_397_mpos, sub(_98, add(expr_397_mpos, 0x20)))
            finalize_allocation(expr_397_mpos, sub(_98, expr_397_mpos))
            /// @src 0:3359,3590
            let expr_398 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_397_mpos), array_length_t_bytes_memory_ptr(expr_397_mpos))
            /// @src 0:3342,3590
            let var_digest_377 := expr_398
            /// @src 0:3637,3643
            let _99 := var_digest_377
            let expr_403 := _99
            /// @src 0:3645,3646
            let _100 := var_v_360
            let expr_404 := _100
            /// @src 0:3648,3649
            let _101 := var_r_362
            let expr_405 := _101
            /// @src 0:3651,3652
            let _102 := var_s_364
            let expr_406 := _102
            /// @src 0:3627,3653

            let _103 := allocate_unbounded()
            let _104 := abi_encode_tuple_t_bytes32_t_uint8_t_bytes32_t_bytes32__to_t_bytes32_t_uint8_t_bytes32_t_bytes32__fromStack(_103 , expr_403, expr_404, expr_405, expr_406)

            mstore(0, 0)

            let _105 := staticcall(gas(), 1 , _103, sub(_104, _103), 0, 32)
            if iszero(_105) { revert_forward_1() }
            let expr_407 := shift_left_0(mload(0))
            /// @src 0:3600,3653
            let var_recoveredAddress_401 := expr_407
            /// @src 0:3671,3687
            let _106 := var_recoveredAddress_401
            let expr_410 := _106
            /// @src 0:3699,3700
            let expr_413 := 0x00
            /// @src 0:3691,3701
            let expr_414 := convert_t_rational_0_by_1_to_t_address(expr_413)
            /// @src 0:3671,3701
            let expr_415 := iszero(eq(cleanup_t_address(expr_410), cleanup_t_address(expr_414)))
            /// @src 0:3671,3730
            let expr_419 := expr_415
            if expr_419 {
                /// @src 0:3705,3721
                let _107 := var_recoveredAddress_401
                let expr_416 := _107
                /// @src 0:3725,3730
                let _108 := var_owner_352
                let expr_417 := _108
                /// @src 0:3705,3730
                let expr_418 := eq(cleanup_t_address(expr_416), cleanup_t_address(expr_417))
                /// @src 0:3671,3730
                expr_419 := expr_418
            }
            /// @src 0:3663,3763
            require_helper_t_stringliteral_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56(expr_419)
            /// @src 0:3773,3781
            let expr_423_functionIdentifier := 217
            /// @src 0:3782,3787
            let _109 := var_owner_352
            let expr_424 := _109
            /// @src 0:3789,3796
            let _110 := var_spender_354
            let expr_425 := _110
            /// @src 0:3798,3803
            let _111 := var_value_356
            let expr_426 := _111
            fun__approve_217(expr_424, expr_425, expr_426)

        }

        function fun_sub_604(var_x_584, var_y_586) -> var_z_589 {
            /// @src 2:286,413
            /// @src 2:338,344
            let zero_t_uint256_116 := zero_value_for_split_t_uint256()
            var_z_589 := zero_t_uint256_116

            /// @src 2:369,370
            let _117 := var_x_584
            let expr_593 := _117
            /// @src 2:373,374
            let _118 := var_y_586
            let expr_594 := _118
            /// @src 2:369,374
            let expr_595 := checked_sub_t_uint256(expr_593, expr_594)
            // invalid()
            /// @src 2:365,374
            var_z_589 := expr_595
            let expr_596 := expr_595
            /// @src 2:364,375
            let expr_597 := expr_596
            /// @src 2:379,380
            let _119 := var_x_584
            let expr_598 := _119
            /// @src 2:364,380
            let expr_599 := iszero(gt(cleanup_t_uint256(expr_597), cleanup_t_uint256(expr_598)))
            /// @src 2:356,406
            require_helper_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590(expr_599)

        }

        function fun_transferFrom_350(var_from_297, var_to_299, var_value_301) -> var__305 {
            /// @src 0:2825,3134
            /// @src 0:2912,2916
            let zero_t_bool_62 := zero_value_for_split_t_bool()
            var__305 := zero_t_bool_62

            /// @src 0:2932,2941
            let _63 := 0x02
            let expr_307 := _63
            /// @src 0:2942,2946
            let _64 := var_from_297
            let expr_308 := _64
            /// @src 0:2932,2947
            let _65 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_307,expr_308)
            let _66 := _65
            let expr_309 := _66
            /// @src 0:2948,2958
            let expr_311 := caller()
            /// @src 0:2932,2959
            let _67 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_309,expr_311)
            let _68 := read_from_storage_split_offset_0_t_uint256(_67)
            let expr_312 := _68
            /// @src 0:2973,2974
            let expr_317 := 0x01
            /// @src 0:2972,2974
            let expr_318 := 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
            /// @src 0:2968,2975
            let expr_319 := convert_t_rational_minus_1_by_1_to_t_int256(expr_318)
            /// @src 0:2963,2976
            let expr_320 := convert_t_int256_to_t_uint256(expr_319)
            /// @src 0:2932,2976
            let expr_321 := iszero(eq(cleanup_t_uint256(expr_312), cleanup_t_uint256(expr_320)))
            /// @src 0:2928,3071
            if expr_321 {
                /// @src 0:3022,3031
                let _69 := 0x02
                let expr_328 := _69
                /// @src 0:3032,3036
                let _70 := var_from_297
                let expr_329 := _70
                /// @src 0:3022,3037
                let _71 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_328,expr_329)
                let _72 := _71
                let expr_330 := _72
                /// @src 0:3038,3048
                let expr_332 := caller()
                /// @src 0:3022,3049
                let _73 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_330,expr_332)
                let _74 := read_from_storage_split_offset_0_t_uint256(_73)
                let expr_333 := _74
                /// @src 0:3022,3053
                let expr_334_self := expr_333
                let expr_334_functionIdentifier := 604
                /// @src 0:3054,3059
                let _75 := var_value_301
                let expr_335 := _75
                /// @src 0:3022,3060
                let expr_336 := fun_sub_604(expr_334_self, expr_335)
                /// @src 0:2992,3001
                let _76 := 0x02
                let expr_322 := _76
                /// @src 0:3002,3006
                let _77 := var_from_297
                let expr_323 := _77
                /// @src 0:2992,3007
                let _78 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_322,expr_323)
                let _79 := _78
                let expr_326 := _79
                /// @src 0:3008,3018
                let expr_325 := caller()
                /// @src 0:2992,3019
                let _80 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_326,expr_325)
                /// @src 0:2992,3060
                update_storage_value_offset_0t_uint256_to_t_uint256(_80, expr_336)
                let expr_337 := expr_336
                /// @src 0:2928,3071
            }
            /// @src 0:3080,3089
            let expr_341_functionIdentifier := 255
            /// @src 0:3090,3094
            let _81 := var_from_297
            let expr_342 := _81
            /// @src 0:3096,3098
            let _82 := var_to_299
            let expr_343 := _82
            /// @src 0:3100,3105
            let _83 := var_value_301
            let expr_344 := _83
            fun__transfer_255(expr_342, expr_343, expr_344)
            /// @src 0:3123,3127
            let expr_347 := 0x01
            /// @src 0:3116,3127
            var__305 := expr_347
            leave

        }

        function fun_transfer_295(var_to_277, var_value_279) -> var__283 {
            /// @src 0:2674,2819
            /// @src 0:2743,2747
            let zero_t_bool_37 := zero_value_for_split_t_bool()
            var__283 := zero_t_bool_37

            /// @src 0:2759,2768
            let expr_285_functionIdentifier := 255
            /// @src 0:2769,2779
            let expr_287 := caller()
            /// @src 0:2781,2783
            let _38 := var_to_277
            let expr_288 := _38
            /// @src 0:2785,2790
            let _39 := var_value_279
            let expr_289 := _39
            fun__transfer_255(expr_287, expr_288, expr_289)
            /// @src 0:2808,2812
            let expr_292 := 0x01
            /// @src 0:2801,2812
            var__283 := expr_292
            leave

        }

        function getter_fun_DOMAIN_SEPARATOR_38() -> ret {
            /// @src 0:516,556

            let slot := 3
            let offset := 0

            ret := read_from_storage_split_dynamic_t_bytes32(slot, offset)

        }

        function getter_fun_PERMIT_TYPEHASH_42() -> ret_0 {
            /// @src 0:666,783
            ret_0 := constant_PERMIT_TYPEHASH_42()
        }

        function getter_fun_allowance_35(key_0, key_1) -> ret {
            /// @src 0:439,509

            let slot := 2
            let offset := 0

            slot := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(slot, key_0)

            slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_1)

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_balanceOf_28(key_0) -> ret {
            /// @src 0:383,433

            let slot := 1
            let offset := 0

            slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_0)

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_decimals_20() -> ret_0 {
            /// @src 0:295,339
            ret_0 := constant_decimals_20()
        }

        function getter_fun_name_12() -> ret_0 {
            /// @src 0:183,234
            ret_0 := constant_name_12()
        }

        function getter_fun_nonces_47(key_0) -> ret {
            /// @src 0:789,836

            let slot := 4
            let offset := 0

            slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_0)

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_symbol_16() -> ret_0 {
            /// @src 0:240,289
            ret_0 := constant_symbol_16()
        }

        function getter_fun_totalSupply_23() -> ret {
            /// @src 0:345,377

            let slot := 0
            let offset := 0

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function increment_t_uint256(value) -> ret {
            value := cleanup_t_uint256(value)
            if eq(value, 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff) { panic_error_0x11() }
            ret := add(value, 1)
        }

        function leftAlign_t_bytes32(value) -> aligned {
            aligned := value
        }

        function mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(slot , key) -> dataSlot {
            mstore(0, convert_t_address_to_t_address(key))
            mstore(0x20, slot)
            dataSlot := keccak256(0, 0x40)
        }

        function mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot , key) -> dataSlot {
            mstore(0, convert_t_address_to_t_address(key))
            mstore(0x20, slot)
            dataSlot := keccak256(0, 0x40)
        }

        function panic_error_0x11() {
            mstore(0, 35408467139433450592217433187231851964531694900788300625387963629091585785856)
            mstore(4, 0x11)
            revert(0, 0x24)
        }

        function panic_error_0x41() {
            mstore(0, 35408467139433450592217433187231851964531694900788300625387963629091585785856)
            mstore(4, 0x41)
            revert(0, 0x24)
        }

        function prepare_store_t_uint256(value) -> ret {
            ret := value
        }

        function read_from_storage_split_dynamic_t_bytes32(slot, offset) -> value {
            value := extract_from_storage_value_dynamict_bytes32(sload(slot), offset)

        }

        function read_from_storage_split_dynamic_t_uint256(slot, offset) -> value {
            value := extract_from_storage_value_dynamict_uint256(sload(slot), offset)

        }

        function read_from_storage_split_offset_0_t_bytes32(slot) -> value {
            value := extract_from_storage_value_offset_0t_bytes32(sload(slot))

        }

        function read_from_storage_split_offset_0_t_uint256(slot) -> value {
            value := extract_from_storage_value_offset_0t_uint256(sload(slot))

        }

        function require_helper_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74() {
            sstore(0x7, 0x42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74)
            revert(0, 0)
        }

        function revert_error_c1322bf8034eace5e0b5c7295db60986aa89aae5e0ea0873e4689e076861a5db() {
            sstore(0x7, 0xc1322bf8034eace5e0b5c7295db60986aa89aae5e0ea0873e4689e076861a5db)
            revert(0, 0)
        }

        function revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() {
            sstore(0x7, 0xca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb)
            revert(0, 0)
        }

        function revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() {
            sstore(0x7, 0xdbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b)
            revert(0, 0)
        }

        function revert_forward_1() {
            let pos := allocate_unbounded()
            returndatacopy(pos, 0, returndatasize())
            revert(pos, returndatasize())
        }

        function round_up_to_mul_of_32(value) -> result {
            result := and(add(value, 31), not(31))
        }

        function shift_left_0(value) -> newValue {
            newValue :=

            shl(0, value)

        }

        function shift_right_0_unsigned(value) -> newValue {
            newValue :=

            shr(0, value)

        }

        function shift_right_224_unsigned(value) -> newValue {
            newValue :=

            shr(224, value)

        }

        function shift_right_unsigned_dynamic(bits, value) -> newValue {
            newValue :=

            shr(bits, value)

        }

        function store_literal_in_memory_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590(memPtr) {

            mstore(add(memPtr, 0), "ds-math-sub-underflow")

        }

        function store_literal_in_memory_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255(memPtr) {

            mstore(add(memPtr, 0), "UNI-V2")

        }

        function store_literal_in_memory_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: INVALID_SIGNATURE")

        }

        function store_literal_in_memory_301a50b291d33ce1e8e9064e3f6a6c51d902ec22892b50d58abf6357c6a45541(memPtr) {

            mstore(add(memPtr, 0), 0x1901000000000000000000000000000000000000000000000000000000000000)

        }

        function store_literal_in_memory_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db(memPtr) {

            mstore(add(memPtr, 0), "ds-math-add-overflow")

        }

        function store_literal_in_memory_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: EXPIRED")

        }

        function store_literal_in_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738(memPtr) {

            mstore(add(memPtr, 0), "Uniswap V2")

        }

        function update_byte_slice_32_shift_0(value, toInsert) -> result {
            let mask := 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
            toInsert := shift_left_0(toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_storage_value_offset_0t_uint256_to_t_uint256(slot, value_0) {
            let convertedValue_0 := convert_t_uint256_to_t_uint256(value_0)
            sstore(slot, update_byte_slice_32_shift_0(sload(slot), prepare_store_t_uint256(convertedValue_0)))
        }

        function validator_revert_t_address(value) {
            if iszero(eq(value, cleanup_t_address(value))) { revert(0, 0) }
        }

        function validator_revert_t_bytes32(value) {
            if iszero(eq(value, cleanup_t_bytes32(value))) { revert(0, 0) }
        }

        function validator_revert_t_uint256(value) {
            if iszero(eq(value, cleanup_t_uint256(value))) { revert(0, 0) }
        }

        function validator_revert_t_uint8(value) {
            if iszero(eq(value, cleanup_t_uint8(value))) { revert(0, 0) }
        }

        function zero_value_for_split_t_bool() -> ret {
            ret := 0
        }

        function zero_value_for_split_t_uint256() -> ret {
            ret := 0
        }

    }

    data ".metadata" hex"a26469706673582212209f10533bb5a1486af58ba1753497a14c34b4911e589bb1419718ece6c28f157264736f6c63430008060033"
}

}
