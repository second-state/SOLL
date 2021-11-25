/*=====================================================*
 *                       WARNING                       *
 *  Solidity to Yul compilation is still EXPERIMENTAL  *
 *       It can result in LOSS OF FUNDS or worse       *
 *                !USE AT YOUR OWN RISK!               *
 *=====================================================*/



object "UniswapV2Pair_1046_deployed" {
    code {
        /// @src 1:289,9957
        mstore(64, 128)

        if iszero(lt(calldatasize(), 4))
        {
            let selector := shift_right_224_unsigned(calldataload(0))
            switch selector

            case 0x022c0d9f
            {
                // swap(uint256,uint256,address,bytes)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1, param_2, param_3, param_4 :=  abi_decode_tuple_t_uint256t_uint256t_addresst_bytes_calldata_ptr(4, calldatasize())
                fun_swap_964(param_0, param_1, param_2, param_3, param_4)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                return(memPos, sub(memEnd, memPos))
            }

            case 0x06fdde03
            {
                // name()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_name_1059()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_string_memory_ptr__to_t_string_memory_ptr__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x0902f1ac
            {
                // getReserves()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0, ret_1, ret_2 :=  fun_getReserves_104()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint112_t_uint112_t_uint32__to_t_uint112_t_uint112_t_uint32__fromStack(memPos , ret_0, ret_1, ret_2)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x095ea7b3
            {
                // approve(address,uint256)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_uint256(4, calldatasize())
                let ret_0 :=  fun_approve_1282(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x0dfe1681
            {
                // token0()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_token0_41()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x18160ddd
            {
                // totalSupply()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_totalSupply_1070()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x23b872dd
            {
                // transferFrom(address,address,uint256)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1, param_2 :=  abi_decode_tuple_t_addresst_addresst_uint256(4, calldatasize())
                let ret_0 :=  fun_transferFrom_1357(param_0, param_1, param_2)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x30adf81f
            {
                // PERMIT_TYPEHASH()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_PERMIT_TYPEHASH_1089()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bytes32__to_t_bytes32__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x313ce567
            {
                // decimals()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_decimals_1067()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint8__to_t_uint8__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x3644e515
            {
                // DOMAIN_SEPARATOR()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_DOMAIN_SEPARATOR_1085()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bytes32__to_t_bytes32__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x485cc955
            {
                // initialize(address,address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_address(4, calldatasize())
                sstore(0x1, param_0)
                sstore(0x2, param_1)
                fun_initialize_181(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                return(memPos, sub(memEnd, memPos))
            }

            case 0x5909c0d5
            {
                // price0CumulativeLast()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_price0CumulativeLast_53()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x5a3d5493
            {
                // price1CumulativeLast()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_price1CumulativeLast_56()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x6a627842
            {
                // mint(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                let ret_0 :=  fun_mint_554(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x70a08231
            {
                // balanceOf(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                let ret_0 :=  getter_fun_balanceOf_1075(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x7464fc3d
            {
                // kLast()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_kLast_59()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x7ecebe00
            {
                // nonces(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                let ret_0 :=  getter_fun_nonces_1094(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x89afcb44
            {
                // burn(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                let ret_0, ret_1 :=  fun_burn_726(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256_t_uint256__to_t_uint256_t_uint256__fromStack(memPos , ret_0, ret_1)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x95d89b41
            {
                // symbol()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_symbol_1063()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_string_memory_ptr__to_t_string_memory_ptr__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xa9059cbb
            {
                // transfer(address,uint256)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_uint256(4, calldatasize())
                let ret_0 :=  fun_transfer_1302(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xba9a7a56
            {
                // MINIMUM_LIQUIDITY()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_MINIMUM_LIQUIDITY_24()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xbc25cf77
            {
                // skim(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                fun_skim_1015(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                return(memPos, sub(memEnd, memPos))
            }

            case 0xc45a0155
            {
                // factory()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_factory_38()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xd21220a7
            {
                // token1()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_token1_44()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xd505accf
            {
                // permit(address,address,uint256,uint256,uint8,bytes32,bytes32)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1, param_2, param_3, param_4, param_5, param_6 :=  abi_decode_tuple_t_addresst_addresst_uint256t_uint256t_uint8t_bytes32t_bytes32(4, calldatasize())
                fun_permit_1437(param_0, param_1, param_2, param_3, param_4, param_5, param_6)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                return(memPos, sub(memEnd, memPos))
            }

            case 0xdd62ed3e
            {
                // allowance(address,address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_address(4, calldatasize())
                let ret_0 :=  getter_fun_allowance_1082(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xfff6cae9
            {
                // sync()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                fun_sync_1045()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                return(memPos, sub(memEnd, memPos))
            }

            default {}
        }
        if iszero(calldatasize()) {  }
        // revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74()

        function abi_decode_t_address(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_address(value)
        }

        function abi_decode_t_address_fromMemory(offset, end) -> value {
            value := mload(offset)
            validator_revert_t_address(value)
        }

        function abi_decode_t_bool_fromMemory(offset, end) -> value {
            value := mload(offset)
            validator_revert_t_bool(value)
        }

        function abi_decode_t_bytes32(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_bytes32(value)
        }

        // bytes
        function abi_decode_t_bytes_calldata_ptr(offset, end) -> arrayPos, length {
            if iszero(slt(add(offset, 0x1f), end)) { revert_error_1b9f4a0a5773e33b91aa01db23bf8c55fce1411167c872835e7fa00a4f17d46d() }
            length := calldataload(offset)
            if gt(length, 0xffffffffffffffff) { revert_error_15abf5612cd996bc235ba1e55a4a30ac60e6bb601ff7ba4ad3f179b6be8d0490() }
            arrayPos := add(offset, 0x20)
            if gt(add(arrayPos, mul(length, 0x01)), end) { revert_error_81385d8c0b31fffe14be1da910c8bd3a80be4cfa248e04f42ec0faea3132a8ef() }
        }

        function abi_decode_t_uint256(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_uint256(value)
        }

        function abi_decode_t_uint256_fromMemory(offset, end) -> value {
            value := mload(offset)
            validator_revert_t_uint256(value)
        }

        function abi_decode_t_uint8(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_uint8(value)
        }

        function abi_decode_tuple_(headStart, dataEnd)   {
            if slt(sub(dataEnd, headStart), 0) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

        }

        function abi_decode_tuple__fromMemory(headStart, dataEnd)   {
            if slt(sub(dataEnd, headStart), 0) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

        }

        function abi_decode_tuple_t_address(headStart, dataEnd) -> value0 {
            if slt(sub(dataEnd, headStart), 32) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

        }

        function abi_decode_tuple_t_address_fromMemory(headStart, dataEnd) -> value0 {
            if slt(sub(dataEnd, headStart), 32) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_address_fromMemory(add(headStart, offset), dataEnd)
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

        function abi_decode_tuple_t_bool_fromMemory(headStart, dataEnd) -> value0 {
            if slt(sub(dataEnd, headStart), 32) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_bool_fromMemory(add(headStart, offset), dataEnd)
            }

        }

        function abi_decode_tuple_t_uint256_fromMemory(headStart, dataEnd) -> value0 {
            if slt(sub(dataEnd, headStart), 32) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_uint256_fromMemory(add(headStart, offset), dataEnd)
            }

        }

        function abi_decode_tuple_t_uint256t_uint256t_addresst_bytes_calldata_ptr(headStart, dataEnd) -> value0, value1, value2, value3, value4 {
            if slt(sub(dataEnd, headStart), 128) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
            }

            {

                let offset := 32

                value1 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
            }

            {

                let offset := 64

                value2 := abi_decode_t_address(add(headStart, offset), dataEnd)
            }

            {

                let offset := calldataload(add(headStart, 96))
                if gt(offset, 0xffffffffffffffff) { revert_error_c1322bf8034eace5e0b5c7295db60986aa89aae5e0ea0873e4689e076861a5db() }

                value3, value4 := abi_decode_t_bytes_calldata_ptr(add(headStart, offset), dataEnd)
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

        // bytes -> bytes
        function abi_encode_t_bytes_calldata_ptr_to_t_bytes_memory_ptr_fromStack(start, length, pos) -> end {
            pos := array_storeLengthForEncoding_t_bytes_memory_ptr_fromStack(pos, length)

            copy_calldata_to_memory(start, pos, length)
            end := add(pos, round_up_to_mul_of_32(length))
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

        function abi_encode_t_stringliteral_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 37)
            store_literal_in_memory_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83(pos)
            end := add(pos, 64)
        }

        function abi_encode_t_stringliteral_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 36)
            store_literal_in_memory_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67(pos)
            end := add(pos, 64)
        }

        function abi_encode_t_stringliteral_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 20)
            store_literal_in_memory_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 21)
            store_literal_in_memory_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3(pos)
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

        function abi_encode_t_stringliteral_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 33)
            store_literal_in_memory_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f(pos)
            end := add(pos, 64)
        }

        function abi_encode_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 18)
            store_literal_in_memory_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 17)
            store_literal_in_memory_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 12)
            store_literal_in_memory_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 40)
            store_literal_in_memory_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562(pos)
            end := add(pos, 64)
        }

        function abi_encode_t_stringliteral_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 40)
            store_literal_in_memory_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e(pos)
            end := add(pos, 64)
        }

        function abi_encode_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 20)
            store_literal_in_memory_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 19)
            store_literal_in_memory_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 26)
            store_literal_in_memory_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_uint112_to_t_uint112_fromStack(value, pos) {
            mstore(pos, cleanup_t_uint112(value))
        }

        function abi_encode_t_uint256_to_t_uint256_fromStack(value, pos) {
            mstore(pos, cleanup_t_uint256(value))
        }

        function abi_encode_t_uint32_to_t_uint32_fromStack(value, pos) {
            mstore(pos, cleanup_t_uint32(value))
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

        function abi_encode_tuple_t_address__to_t_address__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_address_to_t_address_fromStack(value0,  add(headStart, 0))

        }

        function abi_encode_tuple_t_address_t_uint256__to_t_address_t_uint256__fromStack(headStart , value0, value1) -> tail {
            tail := add(headStart, 64)

            abi_encode_t_address_to_t_address_fromStack(value0,  add(headStart, 0))

            abi_encode_t_uint256_to_t_uint256_fromStack(value1,  add(headStart, 32))

        }

        function abi_encode_tuple_t_address_t_uint256_t_uint256_t_bytes_calldata_ptr__to_t_address_t_uint256_t_uint256_t_bytes_memory_ptr__fromStack(headStart , value0, value1, value2, value3, value4) -> tail {
            tail := add(headStart, 128)

            abi_encode_t_address_to_t_address_fromStack(value0,  add(headStart, 0))

            abi_encode_t_uint256_to_t_uint256_fromStack(value1,  add(headStart, 32))

            abi_encode_t_uint256_to_t_uint256_fromStack(value2,  add(headStart, 64))

            mstore(add(headStart, 96), sub(tail, headStart))
            tail := abi_encode_t_bytes_calldata_ptr_to_t_bytes_memory_ptr_fromStack(value3, value4,  tail)

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

        function abi_encode_tuple_t_stringliteral_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3_to_t_string_memory_ptr_fromStack( tail)

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

        function abi_encode_tuple_t_stringliteral_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_uint112_t_uint112__to_t_uint112_t_uint112__fromStack(headStart , value0, value1) -> tail {
            tail := add(headStart, 64)

            abi_encode_t_uint112_to_t_uint112_fromStack(value0,  add(headStart, 0))

            abi_encode_t_uint112_to_t_uint112_fromStack(value1,  add(headStart, 32))

        }

        function abi_encode_tuple_t_uint112_t_uint112_t_uint32__to_t_uint112_t_uint112_t_uint32__fromStack(headStart , value0, value1, value2) -> tail {
            tail := add(headStart, 96)

            abi_encode_t_uint112_to_t_uint112_fromStack(value0,  add(headStart, 0))

            abi_encode_t_uint112_to_t_uint112_fromStack(value1,  add(headStart, 32))

            abi_encode_t_uint32_to_t_uint32_fromStack(value2,  add(headStart, 64))

        }

        function abi_encode_tuple_t_uint256__to_t_uint256__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

        }

        function abi_encode_tuple_t_uint256_t_uint256__to_t_uint256_t_uint256__fromStack(headStart , value0, value1) -> tail {
            tail := add(headStart, 64)

            abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

            abi_encode_t_uint256_to_t_uint256_fromStack(value1,  add(headStart, 32))

        }

        function abi_encode_tuple_t_uint256_t_uint256_t_uint256_t_uint256__to_t_uint256_t_uint256_t_uint256_t_uint256__fromStack(headStart , value0, value1, value2, value3) -> tail {
            tail := add(headStart, 128)

            abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

            abi_encode_t_uint256_to_t_uint256_fromStack(value1,  add(headStart, 32))

            abi_encode_t_uint256_to_t_uint256_fromStack(value2,  add(headStart, 64))

            abi_encode_t_uint256_to_t_uint256_fromStack(value3,  add(headStart, 96))

        }

        function abi_encode_tuple_t_uint8__to_t_uint8__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_uint8_to_t_uint8_fromStack(value0,  add(headStart, 0))

        }

        function allocate_memory(size) -> memPtr {
            memPtr := allocate_unbounded()
            finalize_allocation(memPtr, size)
        }

        function allocate_memory_array_t_bytes_memory_ptr(length) -> memPtr {
            let allocSize := array_allocation_size_t_bytes_memory_ptr(length)
            memPtr := allocate_memory(allocSize)

            mstore(memPtr, length)

        }

        function allocate_memory_array_t_string_memory_ptr(length) -> memPtr {
            let allocSize := array_allocation_size_t_string_memory_ptr(length)
            memPtr := allocate_memory(allocSize)

            mstore(memPtr, length)

        }

        function allocate_unbounded() -> memPtr {
            memPtr := mload(64)
        }

        function array_allocation_size_t_bytes_memory_ptr(length) -> size {
            // Make sure we can allocate memory without overflow
            if gt(length, 0xffffffffffffffff) { panic_error_0x41() }

            size := round_up_to_mul_of_32(length)

            // add length slot
            size := add(size, 0x20)

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

        function array_length_t_bytes_calldata_ptr(value, len) -> length {

            length := len

        }

        function array_length_t_bytes_memory_ptr(value) -> length {

            length := mload(value)

        }

        function array_length_t_string_memory_ptr(value) -> length {

            length := mload(value)

        }

        function array_storeLengthForEncoding_t_bytes_memory_ptr_fromStack(pos, length) -> updated_pos {
            mstore(pos, length)
            updated_pos := add(pos, 0x20)
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

        function checked_div_t_uint224(x, y) -> r {
            x := cleanup_t_uint224(x)
            y := cleanup_t_uint224(y)
            if iszero(y) { panic_error_0x12() }

            r := div(x, y)
        }

        function checked_div_t_uint256(x, y) -> r {
            x := cleanup_t_uint256(x)
            y := cleanup_t_uint256(y)
            if iszero(y) { panic_error_0x12() }

            r := div(x, y)
        }

        function checked_mul_t_uint224(x, y) -> product {
            x := cleanup_t_uint224(x)
            y := cleanup_t_uint224(y)

            // overflow, if x != 0 and y > (maxValue / x)
            if and(iszero(iszero(x)), gt(y, div(0xffffffffffffffffffffffffffffffffffffffffffffffffffffffff, x))) { panic_error_0x11() }

            product := mul(x, y)
        }

        function checked_mul_t_uint256(x, y) -> product {
            x := cleanup_t_uint256(x)
            y := cleanup_t_uint256(y)

            // overflow, if x != 0 and y > (maxValue / x)
            if and(iszero(iszero(x)), gt(y, div(0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff, x))) { panic_error_0x11() }

            product := mul(x, y)
        }

        function checked_sub_t_uint256(x, y) -> diff {
            x := cleanup_t_uint256(x)
            y := cleanup_t_uint256(y)

            if lt(x, y) { panic_error_0x11() }

            diff := sub(x, y)
        }

        function checked_sub_t_uint32(x, y) -> diff {
            x := cleanup_t_uint32(x)
            y := cleanup_t_uint32(y)

            if lt(x, y) { panic_error_0x11() }

            diff := sub(x, y)
        }

        function cleanup_from_storage_t_address(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffff)
        }

        function cleanup_from_storage_t_bytes32(value) -> cleaned {
            cleaned := value
        }

        function cleanup_from_storage_t_uint112(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffff)
        }

        function cleanup_from_storage_t_uint256(value) -> cleaned {
            cleaned := value
        }

        function cleanup_from_storage_t_uint32(value) -> cleaned {
            cleaned := and(value, 0xffffffff)
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

        function cleanup_t_uint112(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffff)
        }

        function cleanup_t_uint160(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffff)
        }

        function cleanup_t_uint224(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffff)
        }

        function cleanup_t_uint256(value) -> cleaned {
            cleaned := value
        }

        function cleanup_t_uint32(value) -> cleaned {
            cleaned := and(value, 0xffffffff)
        }

        function cleanup_t_uint8(value) -> cleaned {
            cleaned := and(value, 0xff)
        }

        /// @src 1:416,471
        function constant_MINIMUM_LIQUIDITY_24() -> ret {
            /// @src 1:466,471
            let expr_23 := 0x03e8
            let _5 := convert_t_rational_1000_by_1_to_t_uint256(expr_23)

            ret := _5
        }

        /// @src 0:666,783
        function constant_PERMIT_TYPEHASH_1089() -> ret {
            /// @src 0:717,783
            let expr_1088 := 0x6e71edae12b1b97f4d1f60370fef10105fa2faae0126114a169c64845d6126c9
            let _2 := convert_t_rational_49955707469362902507454157297736832118868343942642399513960811609542965143241_by_1_to_t_bytes32(expr_1088)

            ret := _2
        }

        /// @src 9:206,236
        function constant_Q112_1809() -> ret {
            /// @src 9:230,236
            let expr_1808 := 0x010000000000000000000000000000
            let _442 := convert_t_rational_5192296858534827628530496329220096_by_1_to_t_uint224(expr_1808)

            ret := _442
        }

        /// @src 1:477,565
        function constant_SELECTOR_35() -> ret {
            /// @src 1:529,563
            let expr_32_mpos := convert_t_stringliteral_a9059cbb2ab09eb219583f4a59a5d0623ade346d962bcd4e46b11da047c9049b_to_t_bytes_memory_ptr()
            /// @src 1:519,564
            let expr_33 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_32_mpos), array_length_t_bytes_memory_ptr(expr_32_mpos))
            /// @src 1:512,565
            let expr_34 := convert_t_bytes32_to_t_bytes4(expr_33)
            let _177 := expr_34

            ret := _177
        }

        /// @src 0:295,339
        function constant_decimals_1067() -> ret {
            /// @src 0:337,339
            let expr_1066 := 0x12
            let _3 := convert_t_rational_18_by_1_to_t_uint8(expr_1066)

            ret := _3
        }

        /// @src 0:183,234
        function constant_name_1059() -> ret_mpos {
            /// @src 0:222,234
            let _1_mpos := convert_t_stringliteral_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738_to_t_string_memory_ptr()

            ret_mpos := _1_mpos
        }

        /// @src 0:240,289
        function constant_symbol_1063() -> ret_mpos {
            /// @src 0:281,289
            let _4_mpos := convert_t_stringliteral_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255_to_t_string_memory_ptr()

            ret_mpos := _4_mpos
        }

        function convert_t_address_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_address(value)
        }

        function convert_t_address_to_t_contract$_IERC20_$1522(value) -> converted {
            converted := convert_t_uint160_to_t_contract$_IERC20_$1522(value)
        }

        function convert_t_address_to_t_contract$_IUniswapV2Callee_$1536(value) -> converted {
            converted := convert_t_uint160_to_t_contract$_IUniswapV2Callee_$1536(value)
        }

        function convert_t_address_to_t_contract$_IUniswapV2Factory_$1599(value) -> converted {
            converted := convert_t_uint160_to_t_contract$_IUniswapV2Factory_$1599(value)
        }

        function convert_t_bytes32_to_t_bytes4(value) -> converted {
            converted := cleanup_t_bytes32(value)
        }

        function convert_t_contract$_IERC20_$1522_to_t_address(value) -> converted {
            converted := convert_t_contract$_IERC20_$1522_to_t_uint160(value)
        }

        function convert_t_contract$_IERC20_$1522_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_contract$_IUniswapV2Callee_$1536_to_t_address(value) -> converted {
            converted := convert_t_contract$_IUniswapV2Callee_$1536_to_t_uint160(value)
        }

        function convert_t_contract$_IUniswapV2Callee_$1536_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_contract$_IUniswapV2Factory_$1599_to_t_address(value) -> converted {
            converted := convert_t_contract$_IUniswapV2Factory_$1599_to_t_uint160(value)
        }

        function convert_t_contract$_IUniswapV2Factory_$1599_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_contract$_UniswapV2Pair_$1046_to_t_address(value) -> converted {
            converted := convert_t_contract$_UniswapV2Pair_$1046_to_t_uint160(value)
        }

        function convert_t_contract$_UniswapV2Pair_$1046_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_int256_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_0_by_1_to_t_address(value) -> converted {
            converted := convert_t_rational_0_by_1_to_t_uint160(value)
        }

        function convert_t_rational_0_by_1_to_t_uint112(value) -> converted {
            converted := cleanup_t_uint112(value)
        }

        function convert_t_rational_0_by_1_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_rational_0_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_0_by_1_to_t_uint32(value) -> converted {
            converted := cleanup_t_uint32(value)
        }

        function convert_t_rational_1000000_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_1000_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_18_by_1_to_t_uint8(value) -> converted {
            converted := cleanup_t_uint8(value)
        }

        function convert_t_rational_1_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_2_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_3_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_4294967296_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_49955707469362902507454157297736832118868343942642399513960811609542965143241_by_1_to_t_bytes32(value) -> converted {
            converted := shift_left_0(cleanup_t_rational_49955707469362902507454157297736832118868343942642399513960811609542965143241_by_1(value))
        }

        function convert_t_rational_5192296858534827628530496329220096_by_1_to_t_uint224(value) -> converted {
            converted := cleanup_t_uint224(value)
        }

        function convert_t_rational_5_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_rational_minus_1_by_1_to_t_int256(value) -> converted {
            converted := cleanup_t_int256(value)
        }

        function convert_t_stringliteral_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255_to_t_string_memory_ptr() -> converted {
            converted := copy_literal_to_memory_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255()
        }

        function convert_t_stringliteral_a9059cbb2ab09eb219583f4a59a5d0623ade346d962bcd4e46b11da047c9049b_to_t_bytes_memory_ptr() -> converted {
            converted := copy_literal_to_memory_a9059cbb2ab09eb219583f4a59a5d0623ade346d962bcd4e46b11da047c9049b()
        }

        function convert_t_stringliteral_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738_to_t_string_memory_ptr() -> converted {
            converted := copy_literal_to_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738()
        }

        function convert_t_uint112_to_t_uint112(value) -> converted {
            converted := cleanup_t_uint112(value)
        }

        function convert_t_uint112_to_t_uint224(value) -> converted {
            converted := cleanup_t_uint112(value)
        }

        function convert_t_uint112_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint112(value)
        }

        function convert_t_uint160_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_uint160(value)
        }

        function convert_t_uint160_to_t_contract$_IERC20_$1522(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_uint160_to_t_contract$_IUniswapV2Callee_$1536(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_uint160_to_t_contract$_IUniswapV2Factory_$1599(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_uint160_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_uint224_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint224(value)
        }

        function convert_t_uint256_to_t_uint112(value) -> converted {
            converted := cleanup_t_uint112(value)
        }

        function convert_t_uint256_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_uint256_to_t_uint32(value) -> converted {
            converted := cleanup_t_uint32(value)
        }

        function convert_t_uint32_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint32(value)
        }

        function convert_t_uint32_to_t_uint32(value) -> converted {
            converted := cleanup_t_uint32(value)
        }

        function copy_calldata_to_memory(src, dst, length) {
            calldatacopy(dst, src, length)
            // clear end
            mstore(add(dst, length), 0)
        }

        function copy_literal_to_memory_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255() -> memPtr {
            memPtr := allocate_memory_array_t_string_memory_ptr(6)
            store_literal_in_memory_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255(add(memPtr, 32))
        }

        function copy_literal_to_memory_a9059cbb2ab09eb219583f4a59a5d0623ade346d962bcd4e46b11da047c9049b() -> memPtr {
            memPtr := allocate_memory_array_t_string_memory_ptr(25)
            store_literal_in_memory_a9059cbb2ab09eb219583f4a59a5d0623ade346d962bcd4e46b11da047c9049b(add(memPtr, 32))
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

        function extract_from_storage_value_dynamict_address(slot_value, offset) -> value {
            value := cleanup_from_storage_t_address(shift_right_unsigned_dynamic(mul(offset, 8), slot_value))
        }

        function extract_from_storage_value_dynamict_bytes32(slot_value, offset) -> value {
            value := cleanup_from_storage_t_bytes32(shift_right_unsigned_dynamic(mul(offset, 8), slot_value))
        }

        function extract_from_storage_value_dynamict_uint256(slot_value, offset) -> value {
            value := cleanup_from_storage_t_uint256(shift_right_unsigned_dynamic(mul(offset, 8), slot_value))
        }

        function extract_from_storage_value_offset_0t_address(slot_value) -> value {
            value := cleanup_from_storage_t_address(shift_right_0_unsigned(slot_value))
        }

        function extract_from_storage_value_offset_0t_bytes32(slot_value) -> value {
            value := cleanup_from_storage_t_bytes32(shift_right_0_unsigned(slot_value))
        }

        function extract_from_storage_value_offset_0t_uint112(slot_value) -> value {
            value := cleanup_from_storage_t_uint112(shift_right_0_unsigned(slot_value))
        }

        function extract_from_storage_value_offset_0t_uint256(slot_value) -> value {
            value := cleanup_from_storage_t_uint256(shift_right_0_unsigned(slot_value))
        }

        function extract_from_storage_value_offset_14t_uint112(slot_value) -> value {
            value := cleanup_from_storage_t_uint112(shift_right_112_unsigned(slot_value))
        }

        function extract_from_storage_value_offset_28t_uint32(slot_value) -> value {
            value := cleanup_from_storage_t_uint32(shift_right_224_unsigned(slot_value))
        }

        function extract_returndata() -> data {

            switch returndatasize()
            case 0 {
                data := zero_value_for_split_t_bytes_memory_ptr()
            }
            default {
                data := allocate_memory_array_t_bytes_memory_ptr(returndatasize())
                returndatacopy(add(data, 0x20), 0, returndatasize())
            }

        }

        function finalize_allocation(memPtr, size) {
            let newFreePtr := add(memPtr, round_up_to_mul_of_32(size))
            // protect against overflow
            if or(gt(newFreePtr, 0xffffffffffffffff), lt(newFreePtr, memPtr)) { panic_error_0x41() }
            mstore(64, newFreePtr)
        }

        function fun__approve_1224(var_owner_1202, var_spender_1204, var_value_1206) {
            /// @src 0:1870,2036

            /// @src 0:1978,1983
            let _335 := var_value_1206
            let expr_1214 := _335
            /// @src 0:1950,1959
            let _336 := 0x02
            let expr_1209 := _336
            /// @src 0:1960,1965
            let _337 := var_owner_1202
            let expr_1210 := _337
            /// @src 0:1950,1966
            let _338 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_1209,expr_1210)
            let _339 := _338
            let expr_1212 := _339
            /// @src 0:1967,1974
            let _340 := var_spender_1204
            let expr_1211 := _340
            /// @src 0:1950,1975
            let _341 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1212,expr_1211)
            /// @src 0:1950,1983
            update_storage_value_offset_0t_uint256_to_t_uint256(_341, expr_1214)
            let expr_1215 := expr_1214
            /// @src 0:2007,2012
            let _342 := var_owner_1202
            let expr_1218 := _342
            /// @src 0:2014,2021
            let _343 := var_spender_1204
            let expr_1219 := _343
            /// @src 0:2023,2028
            let _344 := var_value_1206
            let expr_1220 := _344
            /// @src 0:1998,2029
            let _345 := 0x8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925
            {
                let _346 := allocate_unbounded()
                let _347 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_346 , expr_1220)
                log3(_346, sub(_347, _346) , _345, expr_1218, expr_1219)
            }
        }

        function fun__burn_1200(var_from_1167, var_value_1169) {
            /// @src 0:1659,1864

            /// @src 0:1737,1746
            let _317 := 0x01
            let expr_1175 := _317
            /// @src 0:1747,1751
            let _318 := var_from_1167
            let expr_1176 := _318
            /// @src 0:1737,1752
            let _319 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1175,expr_1176)
            let _320 := read_from_storage_split_offset_0_t_uint256(_319)
            let expr_1177 := _320
            /// @src 0:1737,1756
            let expr_1178_self := expr_1177
            let expr_1178_functionIdentifier := 2010
            /// @src 0:1757,1762
            let _321 := var_value_1169
            let expr_1179 := _321
            /// @src 0:1737,1763
            let expr_1180 := fun_sub_2010(expr_1178_self, expr_1179)
            /// @src 0:1719,1728
            let _322 := 0x01
            let expr_1172 := _322
            /// @src 0:1729,1733
            let _323 := var_from_1167
            let expr_1173 := _323
            /// @src 0:1719,1734
            let _324 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1172,expr_1173)
            /// @src 0:1719,1763
            update_storage_value_offset_0t_uint256_to_t_uint256(_324, expr_1180)
            let expr_1181 := expr_1180
            /// @src 0:1787,1798
            let _325 := read_from_storage_split_offset_0_t_uint256(0x00)
            let expr_1184 := _325
            /// @src 0:1787,1802
            let expr_1185_self := expr_1184
            let expr_1185_functionIdentifier := 2010
            /// @src 0:1803,1808
            let _326 := var_value_1169
            let expr_1186 := _326
            /// @src 0:1787,1809
            let expr_1187 := fun_sub_2010(expr_1185_self, expr_1186)
            /// @src 0:1773,1809
            update_storage_value_offset_0t_uint256_to_t_uint256(0x00, expr_1187)
            let expr_1188 := expr_1187
            /// @src 0:1833,1837
            let _327 := var_from_1167
            let expr_1191 := _327
            /// @src 0:1847,1848
            let expr_1194 := 0x00
            /// @src 0:1839,1849
            let expr_1195 := convert_t_rational_0_by_1_to_t_address(expr_1194)
            /// @src 0:1851,1856
            let _328 := var_value_1169
            let expr_1196 := _328
            /// @src 0:1824,1857
            let _329 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
            {
                let _330 := allocate_unbounded()
                let _331 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_330 , expr_1196)
                log3(_330, sub(_331, _330) , _329, expr_1191, expr_1195)
            }
        }

        function fun__mintFee_404(var__reserve0_298, var__reserve1_300) -> var_feeOn_303 {
            /// @src 1:3663,4482
            /// @src 1:3736,3746
            let zero_t_bool_86 := zero_value_for_split_t_bool()
            var_feeOn_303 := zero_t_bool_86

            /// @src 1:3792,3799
            let _87 := read_from_storage_split_offset_0_t_address(0x05)
            let expr_308 := _87
            /// @src 1:3774,3800
            let expr_309_address := convert_t_address_to_t_contract$_IUniswapV2Factory_$1599(expr_308)
            /// @src 1:3774,3806
            let expr_310_address := convert_t_contract$_IUniswapV2Factory_$1599_to_t_address(expr_309_address)
            let expr_310_functionSelector := 0x017e7e58
            /// @src 1:3774,3808
            if iszero(extcodesize(expr_310_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _88 := allocate_unbounded()
            mstore(_88, shift_left_224(expr_310_functionSelector))
            let _89 := abi_encode_tuple__to__fromStack(add(_88, 4) )

            let _90 := staticcall(gas(), expr_310_address,  _88, sub(_89, _88), _88, 32)

            if iszero(_90) { revert_forward_1() }

            let expr_311
            if _90 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_88, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_311 :=  abi_decode_tuple_t_address_fromMemory(_88, add(_88, returndatasize()))
            }
            /// @src 1:3758,3808
            let var_feeTo_306 := expr_311
            /// @src 1:3826,3831
            let _91 := var_feeTo_306
            let expr_314 := _91
            /// @src 1:3843,3844
            let expr_317 := 0x00
            /// @src 1:3835,3845
            let expr_318 := convert_t_rational_0_by_1_to_t_address(expr_317)
            /// @src 1:3826,3845
            let expr_319 := iszero(eq(cleanup_t_address(expr_314), cleanup_t_address(expr_318)))
            /// @src 1:3818,3845
            var_feeOn_303 := expr_319
            let expr_320 := expr_319
            /// @src 1:3869,3874
            let _92 := read_from_storage_split_offset_0_t_uint256(0x0b)
            let expr_324 := _92
            /// @src 1:3855,3874
            let var__kLast_323 := expr_324
            /// @src 1:3903,3908
            let _93 := var_feeOn_303
            let expr_326 := _93
            /// @src 1:3899,4476
            switch expr_326
            case 0 {
                /// @src 1:4429,4435
                let _94 := var__kLast_323
                let expr_393 := _94
                /// @src 1:4439,4440
                let expr_394 := 0x00
                /// @src 1:4429,4440
                let expr_395 := iszero(eq(cleanup_t_uint256(expr_393), convert_t_rational_0_by_1_to_t_uint256(expr_394)))
                /// @src 1:4425,4476
                if expr_395 {
                    /// @src 1:4464,4465
                    let expr_397 := 0x00
                    /// @src 1:4456,4465
                    let _95 := convert_t_rational_0_by_1_to_t_uint256(expr_397)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0b, _95)
                    let expr_398 := _95
                    /// @src 1:4425,4476
                }
                /// @src 1:3899,4476
            }
            default {
                /// @src 1:3928,3934
                let _96 := var__kLast_323
                let expr_327 := _96
                /// @src 1:3938,3939
                let expr_328 := 0x00
                /// @src 1:3928,3939
                let expr_329 := iszero(eq(cleanup_t_uint256(expr_327), convert_t_rational_0_by_1_to_t_uint256(expr_328)))
                /// @src 1:3924,4409
                if expr_329 {
                    /// @src 1:3972,3976
                    let expr_332_address := linkersymbol("libraries/Math.sol:Math")
                    /// @src 1:3972,3981
                    let expr_333_functionIdentifier := 1801
                    /// @src 1:3987,3996
                    let _97 := var__reserve0_298
                    let expr_336 := _97
                    /// @src 1:3982,3997
                    let expr_337 := convert_t_uint112_to_t_uint256(expr_336)
                    /// @src 1:3982,4001
                    let expr_338_self := expr_337
                    let expr_338_functionIdentifier := 2038
                    /// @src 1:4002,4011
                    let _98 := var__reserve1_300
                    let expr_339 := _98
                    /// @src 1:3982,4012
                    let _99 := convert_t_uint112_to_t_uint256(expr_339)
                    let expr_340 := fun_mul_2038(expr_338_self, _99)
                    /// @src 1:3972,4013
                    let expr_341 := fun_sqrt_1801(expr_340)
                    /// @src 1:3959,4013
                    let var_rootK_331 := expr_341
                    /// @src 1:4048,4052
                    let expr_345_address := linkersymbol("libraries/Math.sol:Math")
                    /// @src 1:4048,4057
                    let expr_346_functionIdentifier := 1801
                    /// @src 1:4058,4064
                    let _100 := var__kLast_323
                    let expr_347 := _100
                    /// @src 1:4048,4065
                    let expr_348 := fun_sqrt_1801(expr_347)
                    /// @src 1:4031,4065
                    let var_rootKLast_344 := expr_348
                    /// @src 1:4087,4092
                    let _101 := var_rootK_331
                    let expr_350 := _101
                    /// @src 1:4095,4104
                    let _102 := var_rootKLast_344
                    let expr_351 := _102
                    /// @src 1:4087,4104
                    let expr_352 := gt(cleanup_t_uint256(expr_350), cleanup_t_uint256(expr_351))
                    /// @src 1:4083,4395
                    if expr_352 {
                        /// @src 1:4145,4156
                        let _103 := read_from_storage_split_offset_0_t_uint256(0x00)
                        let expr_355 := _103
                        /// @src 1:4145,4160
                        let expr_356_self := expr_355
                        let expr_356_functionIdentifier := 2038
                        /// @src 1:4161,4166
                        let _104 := var_rootK_331
                        let expr_357 := _104
                        /// @src 1:4161,4170
                        let expr_358_self := expr_357
                        let expr_358_functionIdentifier := 2010
                        /// @src 1:4171,4180
                        let _105 := var_rootKLast_344
                        let expr_359 := _105
                        /// @src 1:4161,4181
                        let expr_360 := fun_sub_2010(expr_358_self, expr_359)
                        /// @src 1:4145,4182
                        let expr_361 := fun_mul_2038(expr_356_self, expr_360)
                        /// @src 1:4128,4182
                        let var_numerator_354 := expr_361
                        /// @src 1:4223,4228
                        let _106 := var_rootK_331
                        let expr_365 := _106
                        /// @src 1:4223,4232
                        let expr_366_self := expr_365
                        let expr_366_functionIdentifier := 2038
                        /// @src 1:4233,4234
                        let expr_367 := 0x05
                        /// @src 1:4223,4235
                        let _107 := convert_t_rational_5_by_1_to_t_uint256(expr_367)
                        let expr_368 := fun_mul_2038(expr_366_self, _107)
                        /// @src 1:4223,4239
                        let expr_369_self := expr_368
                        let expr_369_functionIdentifier := 1988
                        /// @src 1:4240,4249
                        let _108 := var_rootKLast_344
                        let expr_370 := _108
                        /// @src 1:4223,4250
                        let expr_371 := fun_add_1988(expr_369_self, expr_370)
                        /// @src 1:4204,4250
                        let var_denominator_364 := expr_371
                        /// @src 1:4289,4298
                        let _109 := var_numerator_354
                        let expr_375 := _109
                        /// @src 1:4301,4312
                        let _110 := var_denominator_364
                        let expr_376 := _110
                        /// @src 1:4289,4312
                        let expr_377 := checked_div_t_uint256(expr_375, expr_376)

                        /// @src 1:4272,4312
                        let var_liquidity_374 := expr_377
                        /// @src 1:4338,4347
                        let _111 := var_liquidity_374
                        let expr_379 := _111
                        /// @src 1:4350,4351
                        let expr_380 := 0x00
                        /// @src 1:4338,4351
                        let expr_381 := gt(cleanup_t_uint256(expr_379), convert_t_rational_0_by_1_to_t_uint256(expr_380))
                        /// @src 1:4334,4376
                        if expr_381 {
                            /// @src 1:4353,4358
                            let expr_382_functionIdentifier := 1165
                            /// @src 1:4359,4364
                            let _112 := var_feeTo_306
                            let expr_383 := _112
                            /// @src 1:4366,4375
                            let _113 := var_liquidity_374
                            let expr_384 := _113
                            fun__mint_1165(expr_383, expr_384)
                            /// @src 1:4334,4376
                        }
                        /// @src 1:4083,4395
                    }
                    /// @src 1:3924,4409
                }
                /// @src 1:3899,4476
            }

        }

        function fun__mint_1165(var_to_1132, var_value_1134) {
            /// @src 0:1456,1653

            /// @src 0:1528,1539
            let _302 := read_from_storage_split_offset_0_t_uint256(0x00)
            let expr_1138 := _302
            /// @src 0:1528,1543
            let expr_1139_self := expr_1138
            let expr_1139_functionIdentifier := 1988
            /// @src 0:1544,1549
            let _303 := var_value_1134
            let expr_1140 := _303
            /// @src 0:1528,1550
            let expr_1141 := fun_add_1988(expr_1139_self, expr_1140)
            /// @src 0:1514,1550
            update_storage_value_offset_0t_uint256_to_t_uint256(0x00, expr_1141)
            let expr_1142 := expr_1141
            /// @src 0:1576,1585
            let _304 := 0x01
            let expr_1147 := _304
            /// @src 0:1586,1588
            let _305 := var_to_1132
            let expr_1148 := _305
            /// @src 0:1576,1589
            let _306 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1147,expr_1148)
            let _307 := read_from_storage_split_offset_0_t_uint256(_306)
            let expr_1149 := _307
            /// @src 0:1576,1593
            let expr_1150_self := expr_1149
            let expr_1150_functionIdentifier := 1988
            /// @src 0:1594,1599
            let _308 := var_value_1134
            let expr_1151 := _308
            /// @src 0:1576,1600
            let expr_1152 := fun_add_1988(expr_1150_self, expr_1151)
            /// @src 0:1560,1569
            let _309 := 0x01
            let expr_1144 := _309
            /// @src 0:1570,1572
            let _310 := var_to_1132
            let expr_1145 := _310
            /// @src 0:1560,1573
            let _311 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1144,expr_1145)
            /// @src 0:1560,1600
            update_storage_value_offset_0t_uint256_to_t_uint256(_311, expr_1152)
            let expr_1153 := expr_1152
            /// @src 0:1632,1633
            let expr_1158 := 0x00
            /// @src 0:1624,1634
            let expr_1159 := convert_t_rational_0_by_1_to_t_address(expr_1158)
            /// @src 0:1636,1638
            let _312 := var_to_1132
            let expr_1160 := _312
            /// @src 0:1640,1645
            let _313 := var_value_1134
            let expr_1161 := _313
            /// @src 0:1615,1646
            let _314 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
            {
                let _315 := allocate_unbounded()
                let _316 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_315 , expr_1161)
                log3(_315, sub(_316, _315) , _314, expr_1159, expr_1160)
            }
        }

        function fun__safeTransfer_147(var_token_106, var_to_108, var_value_110) {
            /// @src 1:1588,1872

            /// @src 1:1704,1709
            let _176 := var_token_106
            let expr_117 := _176
            /// @src 1:1704,1714
            let expr_118_address := expr_117
            /// @src 1:1738,1746
            let expr_121 := constant_SELECTOR_35()
            /// @src 1:1748,1750
            let _178 := var_to_108
            let expr_122 := _178
            /// @src 1:1752,1757
            let _179 := var_value_110
            let expr_123 := _179
            /// @src 1:1715,1758

            let expr_124_mpos := allocate_unbounded()
            let _180 := add(expr_124_mpos, 0x20)

            mstore(_180, expr_121)
            _180 := add(_180, 4)

            let _181 := abi_encode_tuple_t_address_t_uint256__to_t_address_t_uint256__fromStack(_180, expr_122, expr_123)
            mstore(expr_124_mpos, sub(_181, add(expr_124_mpos, 0x20)))
            finalize_allocation(expr_124_mpos, sub(_181, expr_124_mpos))
            /// @src 1:1704,1759

            let _182 := add(expr_124_mpos, 0x20)
            let _183 := mload(expr_124_mpos)

            let expr_125_component_1 := call(gas(), expr_118_address,  0,  _182, _183, 0, 0)
            let expr_125_component_2_mpos := extract_returndata()
            /// @src 1:1668,1759
            let var_success_114 := expr_125_component_1
            let var_data_116_mpos := expr_125_component_2_mpos
            /// @src 1:1777,1784
            let _184 := var_success_114
            let expr_128 := _184
            /// @src 1:1777,1834
            let expr_142 := expr_128
            if expr_142 {
                /// @src 1:1789,1793
                let _185_mpos := var_data_116_mpos
                let expr_129_mpos := _185_mpos
                /// @src 1:1789,1800
                let expr_130 := array_length_t_bytes_memory_ptr(expr_129_mpos)
                /// @src 1:1804,1805
                let expr_131 := 0x00
                /// @src 1:1789,1805
                let expr_132 := eq(cleanup_t_uint256(expr_130), convert_t_rational_0_by_1_to_t_uint256(expr_131))
                /// @src 1:1789,1833
                let expr_140 := expr_132
                if iszero(expr_140) {
                    /// @src 1:1820,1824
                    let _186_mpos := var_data_116_mpos
                    let expr_135_mpos := _186_mpos
                    /// @src 1:1809,1833

                    let expr_139 :=  abi_decode_tuple_t_bool_fromMemory(add(expr_135_mpos, 32), add(add(expr_135_mpos, 32), array_length_t_bytes_memory_ptr(expr_135_mpos)))
                    /// @src 1:1789,1833
                    expr_140 := expr_139
                }
                /// @src 1:1788,1834
                let expr_141 := expr_140
                /// @src 1:1777,1834
                expr_142 := expr_141
            }
            /// @src 1:1769,1865
            require_helper_t_stringliteral_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2(expr_142)

        }

        function fun__transfer_1262(var_from_1226, var_to_1228, var_value_1230) {
            /// @src 0:2042,2258

            /// @src 0:2135,2144
            let _351 := 0x01
            let expr_1236 := _351
            /// @src 0:2145,2149
            let _352 := var_from_1226
            let expr_1237 := _352
            /// @src 0:2135,2150
            let _353 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1236,expr_1237)
            let _354 := read_from_storage_split_offset_0_t_uint256(_353)
            let expr_1238 := _354
            /// @src 0:2135,2154
            let expr_1239_self := expr_1238
            let expr_1239_functionIdentifier := 2010
            /// @src 0:2155,2160
            let _355 := var_value_1230
            let expr_1240 := _355
            /// @src 0:2135,2161
            let expr_1241 := fun_sub_2010(expr_1239_self, expr_1240)
            /// @src 0:2117,2126
            let _356 := 0x01
            let expr_1233 := _356
            /// @src 0:2127,2131
            let _357 := var_from_1226
            let expr_1234 := _357
            /// @src 0:2117,2132
            let _358 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1233,expr_1234)
            /// @src 0:2117,2161
            update_storage_value_offset_0t_uint256_to_t_uint256(_358, expr_1241)
            let expr_1242 := expr_1241
            /// @src 0:2187,2196
            let _359 := 0x01
            let expr_1247 := _359
            /// @src 0:2197,2199
            let _360 := var_to_1228
            let expr_1248 := _360
            /// @src 0:2187,2200
            let _361 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1247,expr_1248)
            let _362 := read_from_storage_split_offset_0_t_uint256(_361)
            let expr_1249 := _362
            /// @src 0:2187,2204
            let expr_1250_self := expr_1249
            let expr_1250_functionIdentifier := 1988
            /// @src 0:2205,2210
            let _363 := var_value_1230
            let expr_1251 := _363
            /// @src 0:2187,2211
            let expr_1252 := fun_add_1988(expr_1250_self, expr_1251)
            /// @src 0:2171,2180
            let _364 := 0x01
            let expr_1244 := _364
            /// @src 0:2181,2183
            let _365 := var_to_1228
            let expr_1245 := _365
            /// @src 0:2171,2184
            let _366 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1244,expr_1245)
            /// @src 0:2171,2211
            update_storage_value_offset_0t_uint256_to_t_uint256(_366, expr_1252)
            let expr_1253 := expr_1252
            /// @src 0:2235,2239
            let _367 := var_from_1226
            let expr_1256 := _367
            /// @src 0:2241,2243
            let _368 := var_to_1228
            let expr_1257 := _368
            /// @src 0:2245,2250
            let _369 := var_value_1230
            let expr_1258 := _369
            /// @src 0:2226,2251
            let _370 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
            {
                let _371 := allocate_unbounded()
                let _372 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_371 , expr_1258)
                log3(_371, sub(_372, _371) , _370, expr_1256, expr_1257)
            }
        }

        function fun__update_296(var_balance0_183, var_balance1_185, var__reserve0_187, var__reserve1_189) {
            /// @src 1:2717,3576

            /// @src 1:2828,2836
            let _63 := var_balance0_183
            let expr_193 := _63
            /// @src 1:2840,2857
            let expr_198 := 0xffffffffffffffffffffffffffff
            /// @src 1:2828,2857
            let expr_199 := iszero(gt(cleanup_t_uint256(expr_193), convert_t_uint112_to_t_uint256(expr_198)))
            /// @src 1:2828,2890
            let expr_207 := expr_199
            if expr_207 {
                /// @src 1:2861,2869
                let _64 := var_balance1_185
                let expr_200 := _64
                /// @src 1:2873,2890
                let expr_205 := 0xffffffffffffffffffffffffffff
                /// @src 1:2861,2890
                let expr_206 := iszero(gt(cleanup_t_uint256(expr_200), convert_t_uint112_to_t_uint256(expr_205)))
                /// @src 1:2828,2890
                expr_207 := expr_206
            }
            /// @src 1:2820,2914
            require_helper_t_stringliteral_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928(expr_207)
            /// @src 1:2955,2970
            let expr_216 := timestamp()
            /// @src 1:2973,2978
            let expr_219 := 0x0100000000
            /// @src 1:2955,2978
            let expr_220 := mod_t_uint256(expr_216, convert_t_rational_4294967296_by_1_to_t_uint256(expr_219))

            /// @src 1:2948,2979
            let expr_221 := convert_t_uint256_to_t_uint32(expr_220)
            /// @src 1:2924,2979
            let var_blockTimestamp_212 := expr_221
            /// @src 1:3010,3024
            let _65 := var_blockTimestamp_212
            let expr_225 := _65
            /// @src 1:3027,3045
            let _66 := read_from_storage_split_offset_28_t_uint32(0x08)
            let expr_226 := _66
            /// @src 1:3010,3045
            let expr_227 := checked_sub_t_uint32(expr_225, expr_226)

            /// @src 1:2989,3045
            let var_timeElapsed_224 := expr_227
            /// @src 1:3082,3093
            let _67 := var_timeElapsed_224
            let expr_229 := _67
            /// @src 1:3096,3097
            let expr_230 := 0x00
            /// @src 1:3082,3097
            let expr_231 := gt(cleanup_t_uint32(expr_229), convert_t_rational_0_by_1_to_t_uint32(expr_230))
            /// @src 1:3082,3115
            let expr_235 := expr_231
            if expr_235 {
                /// @src 1:3101,3110
                let _68 := var__reserve0_187
                let expr_232 := _68
                /// @src 1:3114,3115
                let expr_233 := 0x00
                /// @src 1:3101,3115
                let expr_234 := iszero(eq(cleanup_t_uint112(expr_232), convert_t_rational_0_by_1_to_t_uint112(expr_233)))
                /// @src 1:3082,3115
                expr_235 := expr_234
            }
            /// @src 1:3082,3133
            let expr_239 := expr_235
            if expr_239 {
                /// @src 1:3119,3128
                let _69 := var__reserve1_189
                let expr_236 := _69
                /// @src 1:3132,3133
                let expr_237 := 0x00
                /// @src 1:3119,3133
                let expr_238 := iszero(eq(cleanup_t_uint112(expr_236), convert_t_rational_0_by_1_to_t_uint112(expr_237)))
                /// @src 1:3082,3133
                expr_239 := expr_238
            }
            /// @src 1:3078,3410
            if expr_239 {
                /// @src 1:3238,3247
                let expr_243_address := linkersymbol("libraries/UQ112x112.sol:UQ112x112")
                /// @src 1:3238,3254
                let expr_244_functionIdentifier := 1826
                /// @src 1:3255,3264
                let _70 := var__reserve1_189
                let expr_245 := _70
                /// @src 1:3238,3265
                let expr_246 := fun_encode_1826(expr_245)
                /// @src 1:3238,3271
                let expr_247_self := expr_246
                let expr_247_functionIdentifier := 1845
                /// @src 1:3272,3281
                let _71 := var__reserve0_187
                let expr_248 := _71
                /// @src 1:3238,3282
                let expr_249 := fun_uqdiv_1845(expr_247_self, expr_248)
                /// @src 1:3233,3283
                let expr_250 := convert_t_uint224_to_t_uint256(expr_249)
                /// @src 1:3286,3297
                let _72 := var_timeElapsed_224
                let expr_251 := _72
                /// @src 1:3233,3297
                let expr_252 := checked_mul_t_uint256(expr_250, convert_t_uint32_to_t_uint256(expr_251))

                /// @src 1:3209,3297
                let _73 := read_from_storage_split_offset_0_t_uint256(0x09)
                let expr_253 := checked_add_t_uint256(_73, expr_252)

                update_storage_value_offset_0t_uint256_to_t_uint256(0x09, expr_253)
                /// @src 1:3340,3349
                let expr_258_address := linkersymbol("libraries/UQ112x112.sol:UQ112x112")
                /// @src 1:3340,3356
                let expr_259_functionIdentifier := 1826
                /// @src 1:3357,3366
                let _74 := var__reserve0_187
                let expr_260 := _74
                /// @src 1:3340,3367
                let expr_261 := fun_encode_1826(expr_260)
                /// @src 1:3340,3373
                let expr_262_self := expr_261
                let expr_262_functionIdentifier := 1845
                /// @src 1:3374,3383
                let _75 := var__reserve1_189
                let expr_263 := _75
                /// @src 1:3340,3384
                let expr_264 := fun_uqdiv_1845(expr_262_self, expr_263)
                /// @src 1:3335,3385
                let expr_265 := convert_t_uint224_to_t_uint256(expr_264)
                /// @src 1:3388,3399
                let _76 := var_timeElapsed_224
                let expr_266 := _76
                /// @src 1:3335,3399
                let expr_267 := checked_mul_t_uint256(expr_265, convert_t_uint32_to_t_uint256(expr_266))

                /// @src 1:3311,3399
                let _77 := read_from_storage_split_offset_0_t_uint256(0x0a)
                let expr_268 := checked_add_t_uint256(_77, expr_267)

                update_storage_value_offset_0t_uint256_to_t_uint256(0x0a, expr_268)
                /// @src 1:3078,3410
            }
            /// @src 1:3438,3446
            let _78 := var_balance0_183
            let expr_275 := _78
            /// @src 1:3430,3447
            let expr_276 := convert_t_uint256_to_t_uint112(expr_275)
            /// @src 1:3419,3447
            update_storage_value_offset_0t_uint112_to_t_uint112(0x08, expr_276)
            let expr_277 := expr_276
            /// @src 1:3476,3484
            let _79 := var_balance1_185
            let expr_282 := _79
            /// @src 1:3468,3485
            let expr_283 := convert_t_uint256_to_t_uint112(expr_282)
            /// @src 1:3457,3485
            update_storage_value_offset_14t_uint112_to_t_uint112(0x08, expr_283)
            let expr_284 := expr_283
            /// @src 1:3516,3530
            let _80 := var_blockTimestamp_212
            let expr_287 := _80
            /// @src 1:3495,3530
            update_storage_value_offset_28t_uint32_to_t_uint32(0x08, expr_287)
            let expr_288 := expr_287
            /// @src 1:3550,3558
            let _81 := read_from_storage_split_offset_0_t_uint112(0x08)
            let expr_291 := _81
            /// @src 1:3560,3568
            let _82 := read_from_storage_split_offset_14_t_uint112(0x08)
            let expr_292 := _82
            /// @src 1:3545,3569
            let _83 := 0x1c411e9a96e071241c2f21f7726b17ae89e3cab4c78be50e062b03a9fffbbad1
            {
                let _84 := allocate_unbounded()
                let _85 := abi_encode_tuple_t_uint112_t_uint112__to_t_uint112_t_uint112__fromStack(_84 , expr_291, expr_292)
                log1(_84, sub(_85, _84) , _83)
            }
        }

        function fun_add_1988(var_x_1968, var_y_1970) -> var_z_1973 {
            /// @src 8:154,280
            /// @src 8:206,212
            let zero_t_uint256_446 := zero_value_for_split_t_uint256()
            var_z_1973 := zero_t_uint256_446

            /// @src 8:237,238
            let _447 := var_x_1968
            let expr_1977 := _447
            /// @src 8:241,242
            let _448 := var_y_1970
            let expr_1978 := _448
            /// @src 8:237,242
            let expr_1979 := checked_add_t_uint256(expr_1977, expr_1978)

            /// @src 8:233,242
            var_z_1973 := expr_1979
            let expr_1980 := expr_1979
            /// @src 8:232,243
            let expr_1981 := expr_1980
            /// @src 8:247,248
            let _449 := var_x_1968
            let expr_1982 := _449
            /// @src 8:232,248
            let expr_1983 := iszero(lt(cleanup_t_uint256(expr_1981), cleanup_t_uint256(expr_1982)))
            /// @src 8:224,273
            require_helper_t_stringliteral_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db(expr_1983)

        }

        function fun_approve_1282(var_spender_1264, var_value_1266) -> var__1270 {
            /// @src 0:2264,2417
            /// @src 0:2337,2341
            let zero_t_bool_332 := zero_value_for_split_t_bool()
            var__1270 := zero_t_bool_332

            /// @src 0:2353,2361
            let expr_1272_functionIdentifier := 1224
            /// @src 0:2362,2372
            let expr_1274 := caller()
            /// @src 0:2374,2381
            let _333 := var_spender_1264
            let expr_1275 := _333
            /// @src 0:2383,2388
            let _334 := var_value_1266
            let expr_1276 := _334
            fun__approve_1224(expr_1274, expr_1275, expr_1276)
            /// @src 0:2406,2410
            let expr_1279 := 0x01
            /// @src 0:2399,2410
            var__1270 := expr_1279
            leave

        }

        function fun_burn_726(var_to_556) -> var_amount0_562, var_amount1_564 {
            /// @src 1:5928,7381
            /// @src 1:5986,5998
            let zero_t_uint256_114 := zero_value_for_split_t_uint256()
            var_amount0_562 := zero_t_uint256_114
            /// @src 1:6000,6012
            let zero_t_uint256_115 := zero_value_for_split_t_uint256()
            var_amount1_564 := zero_t_uint256_115

            var_amount0_562, var_amount1_564 := modifier_lock_560(var_amount0_562, var_amount1_564, var_to_556)
        }

        function fun_burn_726_inner(_121, _122, var_to_556) -> var_amount0_562, var_amount1_564 {
            /// @src 1:5928,7381
            var_amount1_564 := _121
            var_amount1_564 := _122

            /// @src 1:6066,6077
            let expr_570_functionIdentifier := 104
            /// @src 1:6066,6079
            let expr_571_component_1, expr_571_component_2, expr_571_component_3 := fun_getReserves_104()
            /// @src 1:6024,6079
            let var__reserve0_567 := expr_571_component_1
            let var__reserve1_569 := expr_571_component_2
            /// @src 1:6122,6128
            let _123 := read_from_storage_split_offset_0_t_address(0x06)
            let expr_575 := _123
            /// @src 1:6104,6128
            let var__token0_574 := expr_575
            /// @src 1:6202,6208
            let _124 := read_from_storage_split_offset_0_t_address(0x07)
            let expr_579 := _124
            /// @src 1:6184,6208
            let var__token1_578 := expr_579
            /// @src 1:6287,6294
            let _125 := var__token0_574
            let expr_584 := _125
            /// @src 1:6280,6295
            let expr_585_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_584)
            /// @src 1:6280,6305
            let expr_586_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_585_address)
            let expr_586_functionSelector := 0x70a08231
            /// @src 1:6314,6318
            let expr_589_address := address()
            /// @src 1:6306,6319
            let expr_590 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_589_address)
            /// @src 1:6280,6320
            if iszero(extcodesize(expr_586_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _126 := allocate_unbounded()
            mstore(_126, shift_left_224(expr_586_functionSelector))
            let _127 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_126, 4) , expr_590)

            let _128 := staticcall(gas(), expr_586_address,  _126, sub(_127, _126), _126, 32)

            if iszero(_128) { revert_forward_1() }

            let expr_591
            if _128 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_126, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_591 :=  abi_decode_tuple_t_uint256_fromMemory(_126, add(_126, returndatasize()))
            }
            /// @src 1:6264,6320
            let var_balance0_582 := expr_591
            /// @src 1:6353,6360
            let _129 := var__token1_578
            let expr_596 := _129
            /// @src 1:6346,6361
            let expr_597_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_596)
            /// @src 1:6346,6371
            let expr_598_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_597_address)
            let expr_598_functionSelector := 0x70a08231
            /// @src 1:6380,6384
            let expr_601_address := address()
            /// @src 1:6372,6385
            let expr_602 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_601_address)
            /// @src 1:6346,6386
            if iszero(extcodesize(expr_598_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _130 := allocate_unbounded()
            mstore(_130, shift_left_224(expr_598_functionSelector))
            let _131 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_130, 4) , expr_602)

            let _132 := staticcall(gas(), expr_598_address,  _130, sub(_131, _130), _130, 32)

            if iszero(_132) { revert_forward_1() }

            let expr_603
            if _132 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_130, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_603 :=  abi_decode_tuple_t_uint256_fromMemory(_130, add(_130, returndatasize()))
            }
            /// @src 1:6330,6386
            let var_balance1_594 := expr_603
            /// @src 1:6413,6422
            let _133 := 0x01
            let expr_607 := _133
            /// @src 1:6431,6435
            let expr_610_address := address()
            /// @src 1:6423,6436
            let expr_611 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_610_address)
            /// @src 1:6413,6437
            let _134 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_607,expr_611)
            let _135 := read_from_storage_split_offset_0_t_uint256(_134)
            let expr_612 := _135
            /// @src 1:6396,6437
            let var_liquidity_606 := expr_612
            /// @src 1:6461,6469
            let expr_616_functionIdentifier := 404
            /// @src 1:6470,6479
            let _136 := var__reserve0_567
            let expr_617 := _136
            /// @src 1:6481,6490
            let _137 := var__reserve1_569
            let expr_618 := _137
            /// @src 1:6461,6491
            let expr_619 := fun__mintFee_404(expr_617, expr_618)
            /// @src 1:6448,6491
            let var_feeOn_615 := expr_619
            /// @src 1:6521,6532
            let _138 := read_from_storage_split_offset_0_t_uint256(0x00)
            let expr_623 := _138
            /// @src 1:6501,6532
            let var__totalSupply_622 := expr_623
            /// @src 1:6630,6639
            let _139 := var_liquidity_606
            let expr_626 := _139
            /// @src 1:6630,6643
            let expr_627_self := expr_626
            let expr_627_functionIdentifier := 2038
            /// @src 1:6644,6652
            let _140 := var_balance0_582
            let expr_628 := _140
            /// @src 1:6630,6653
            let expr_629 := fun_mul_2038(expr_627_self, expr_628)
            /// @src 1:6656,6668
            let _141 := var__totalSupply_622
            let expr_630 := _141
            /// @src 1:6630,6668
            let expr_631 := checked_div_t_uint256(expr_629, expr_630)

            /// @src 1:6620,6668
            var_amount0_562 := expr_631
            let expr_632 := expr_631
            /// @src 1:6736,6745
            let _142 := var_liquidity_606
            let expr_635 := _142
            /// @src 1:6736,6749
            let expr_636_self := expr_635
            let expr_636_functionIdentifier := 2038
            /// @src 1:6750,6758
            let _143 := var_balance1_594
            let expr_637 := _143
            /// @src 1:6736,6759
            let expr_638 := fun_mul_2038(expr_636_self, expr_637)
            /// @src 1:6762,6774
            let _144 := var__totalSupply_622
            let expr_639 := _144
            /// @src 1:6736,6774
            let expr_640 := checked_div_t_uint256(expr_638, expr_639)

            /// @src 1:6726,6774
            var_amount1_564 := expr_640
            let expr_641 := expr_640
            /// @src 1:6840,6847
            let _145 := var_amount0_562
            let expr_644 := _145
            /// @src 1:6850,6851
            let expr_645 := 0x00
            /// @src 1:6840,6851
            let expr_646 := gt(cleanup_t_uint256(expr_644), convert_t_rational_0_by_1_to_t_uint256(expr_645))
            /// @src 1:6840,6866
            let expr_650 := expr_646
            if expr_650 {
                /// @src 1:6855,6862
                let _146 := var_amount1_564
                let expr_647 := _146
                /// @src 1:6865,6866
                let expr_648 := 0x00
                /// @src 1:6855,6866
                let expr_649 := gt(cleanup_t_uint256(expr_647), convert_t_rational_0_by_1_to_t_uint256(expr_648))
                /// @src 1:6840,6866
                expr_650 := expr_649
            }
            /// @src 1:6832,6911
            require_helper_t_stringliteral_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562(expr_650)
            /// @src 1:6921,6926
            let expr_654_functionIdentifier := 1200
            /// @src 1:6935,6939
            let expr_657_address := address()
            /// @src 1:6927,6940
            let expr_658 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_657_address)
            /// @src 1:6942,6951
            let _147 := var_liquidity_606
            let expr_659 := _147
            fun__burn_1200(expr_658, expr_659)
            /// @src 1:6962,6975
            let expr_662_functionIdentifier := 147
            /// @src 1:6976,6983
            let _148 := var__token0_574
            let expr_663 := _148
            /// @src 1:6985,6987
            let _149 := var_to_556
            let expr_664 := _149
            /// @src 1:6989,6996
            let _150 := var_amount0_562
            let expr_665 := _150
            fun__safeTransfer_147(expr_663, expr_664, expr_665)
            /// @src 1:7007,7020
            let expr_668_functionIdentifier := 147
            /// @src 1:7021,7028
            let _151 := var__token1_578
            let expr_669 := _151
            /// @src 1:7030,7032
            let _152 := var_to_556
            let expr_670 := _152
            /// @src 1:7034,7041
            let _153 := var_amount1_564
            let expr_671 := _153
            fun__safeTransfer_147(expr_669, expr_670, expr_671)
            /// @src 1:7070,7077
            let _154 := var__token0_574
            let expr_676 := _154
            /// @src 1:7063,7078
            let expr_677_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_676)
            /// @src 1:7063,7088
            let expr_678_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_677_address)
            let expr_678_functionSelector := 0x70a08231
            /// @src 1:7097,7101
            let expr_681_address := address()
            /// @src 1:7089,7102
            let expr_682 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_681_address)
            /// @src 1:7063,7103
            if iszero(extcodesize(expr_678_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _155 := allocate_unbounded()
            mstore(_155, shift_left_224(expr_678_functionSelector))
            let _156 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_155, 4) , expr_682)

            let _157 := staticcall(gas(), expr_678_address,  _155, sub(_156, _155), _155, 32)

            if iszero(_157) { revert_forward_1() }

            let expr_683
            if _157 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_155, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_683 :=  abi_decode_tuple_t_uint256_fromMemory(_155, add(_155, returndatasize()))
            }
            /// @src 1:7052,7103
            var_balance0_582 := expr_683
            let expr_684 := expr_683
            /// @src 1:7131,7138
            let _158 := var__token1_578
            let expr_688 := _158
            /// @src 1:7124,7139
            let expr_689_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_688)
            /// @src 1:7124,7149
            let expr_690_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_689_address)
            let expr_690_functionSelector := 0x70a08231
            /// @src 1:7158,7162
            let expr_693_address := address()
            /// @src 1:7150,7163
            let expr_694 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_693_address)
            /// @src 1:7124,7164
            if iszero(extcodesize(expr_690_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _159 := allocate_unbounded()
            mstore(_159, shift_left_224(expr_690_functionSelector))
            let _160 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_159, 4) , expr_694)

            let _161 := staticcall(gas(), expr_690_address,  _159, sub(_160, _159), _159, 32)

            if iszero(_161) { revert_forward_1() }

            let expr_695
            if _161 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_159, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_695 :=  abi_decode_tuple_t_uint256_fromMemory(_159, add(_159, returndatasize()))
            }
            /// @src 1:7113,7164
            var_balance1_594 := expr_695
            let expr_696 := expr_695
            /// @src 1:7175,7182
            let expr_698_functionIdentifier := 296
            /// @src 1:7183,7191
            let _162 := var_balance0_582
            let expr_699 := _162
            /// @src 1:7193,7201
            let _163 := var_balance1_594
            let expr_700 := _163
            /// @src 1:7203,7212
            let _164 := var__reserve0_567
            let expr_701 := _164
            /// @src 1:7214,7223
            let _165 := var__reserve1_569
            let expr_702 := _165
            fun__update_296(expr_699, expr_700, expr_701, expr_702)
            /// @src 1:7238,7243
            let _166 := var_feeOn_615
            let expr_705 := _166
            /// @src 1:7234,7281
            if expr_705 {
                /// @src 1:7258,7266
                let _167 := read_from_storage_split_offset_0_t_uint112(0x08)
                let expr_709 := _167
                /// @src 1:7253,7267
                let expr_710 := convert_t_uint112_to_t_uint256(expr_709)
                /// @src 1:7253,7271
                let expr_711_self := expr_710
                let expr_711_functionIdentifier := 2038
                /// @src 1:7272,7280
                let _168 := read_from_storage_split_offset_14_t_uint112(0x08)
                let expr_712 := _168
                /// @src 1:7253,7281
                let _169 := convert_t_uint112_to_t_uint256(expr_712)
                let expr_713 := fun_mul_2038(expr_711_self, _169)
                /// @src 1:7245,7281
                update_storage_value_offset_0t_uint256_to_t_uint256(0x0b, expr_713)
                let expr_714 := expr_713
                /// @src 1:7234,7281
            }
            /// @src 1:7341,7351
            let expr_719 := caller()
            /// @src 1:7353,7360
            let _170 := var_amount0_562
            let expr_720 := _170
            /// @src 1:7362,7369
            let _171 := var_amount1_564
            let expr_721 := _171
            /// @src 1:7371,7373
            let _172 := var_to_556
            let expr_722 := _172
            /// @src 1:7336,7374
            let _173 := 0xdccd412f0b1252819cb1fd330b93224ca42612892bb3f4f789976e6d81936496
            {
                let _174 := allocate_unbounded()
                let _175 := abi_encode_tuple_t_uint256_t_uint256__to_t_uint256_t_uint256__fromStack(_174 , expr_720, expr_721)
                log3(_174, sub(_175, _174) , _173, expr_719, expr_722)
            }
        }

        function fun_encode_1826(var_y_1811) -> var_z_1814 {
            /// @src 9:282,400
            /// @src 9:332,341
            let zero_t_uint224_440 := zero_value_for_split_t_uint224()
            var_z_1814 := zero_t_uint224_440

            /// @src 9:365,366
            let _441 := var_y_1811
            let expr_1819 := _441
            /// @src 9:357,367
            let expr_1820 := convert_t_uint112_to_t_uint224(expr_1819)
            /// @src 9:370,374
            let expr_1821 := constant_Q112_1809()
            /// @src 9:357,374
            let expr_1822 := checked_mul_t_uint224(expr_1820, expr_1821)

            /// @src 9:353,374
            var_z_1814 := expr_1822
            let expr_1823 := expr_1822

        }

        function fun_getReserves_104() -> var__reserve0_85, var__reserve1_87, var__blockTimestampLast_89 {
            /// @src 1:1346,1582
            /// @src 1:1399,1416
            let zero_t_uint112_6 := zero_value_for_split_t_uint112()
            var__reserve0_85 := zero_t_uint112_6
            /// @src 1:1418,1435
            let zero_t_uint112_7 := zero_value_for_split_t_uint112()
            var__reserve1_87 := zero_t_uint112_7
            /// @src 1:1437,1463
            let zero_t_uint32_8 := zero_value_for_split_t_uint32()
            var__blockTimestampLast_89 := zero_t_uint32_8

            /// @src 1:1487,1495
            let _9 := read_from_storage_split_offset_0_t_uint112(0x08)
            let expr_92 := _9
            /// @src 1:1475,1495
            var__reserve0_85 := expr_92
            let expr_93 := expr_92
            /// @src 1:1517,1525
            let _10 := read_from_storage_split_offset_14_t_uint112(0x08)
            let expr_96 := _10
            /// @src 1:1505,1525
            var__reserve1_87 := expr_96
            let expr_97 := expr_96
            /// @src 1:1557,1575
            let _11 := read_from_storage_split_offset_28_t_uint32(0x08)
            let expr_100 := _11
            /// @src 1:1535,1575
            var__blockTimestampLast_89 := expr_100
            let expr_101 := expr_100

        }

        function fun_initialize_181(var__token0_158, var__token1_160) {
            /// @src 1:2420,2635

            /// @src 1:2510,2520
            let expr_166 := caller()
            /// @src 1:2524,2531
            let _12 := read_from_storage_split_offset_0_t_address(0x05)
            let expr_167 := _12
            /// @src 1:2510,2531
            let expr_168 := eq(cleanup_t_address(expr_166), cleanup_t_address(expr_167))
            /// @src 1:2502,2556
            require_helper_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(expr_168)
            /// @src 1:2595,2602
            let _13 := var__token0_158
            let expr_173 := _13
            /// @src 1:2586,2602
            update_storage_value_offset_0t_address_to_t_address(0x06, expr_173)
            let expr_174 := expr_173
            /// @src 1:2621,2628
            let _14 := var__token1_160
            let expr_177 := _14
            /// @src 1:2612,2628
            update_storage_value_offset_0t_address_to_t_address(0x07, expr_177)
            let expr_178 := expr_177

        }

        function fun_min_1747(var_x_1730, var_y_1732) -> var_z_1735 {
            /// @src 7:97,191
            /// @src 7:149,155
            let zero_t_uint256_423 := zero_value_for_split_t_uint256()
            var_z_1735 := zero_t_uint256_423

            /// @src 7:171,172
            let _424 := var_x_1730
            let expr_1738 := _424
            /// @src 7:175,176
            let _425 := var_y_1732
            let expr_1739 := _425
            /// @src 7:171,176
            let expr_1740 := lt(cleanup_t_uint256(expr_1738), cleanup_t_uint256(expr_1739))
            /// @src 7:171,184
            let expr_1743
            switch expr_1740
            case 0 {
                /// @src 7:183,184
                let _426 := var_y_1732
                let expr_1742 := _426
                /// @src 7:171,184
                expr_1743 := expr_1742
            }
            default {
                /// @src 7:179,180
                let _427 := var_x_1730
                let expr_1741 := _427
                /// @src 7:171,184
                expr_1743 := expr_1741
            }
            /// @src 7:167,184
            var_z_1735 := expr_1743
            let expr_1744 := expr_1743

        }

        function fun_mint_554(var_to_406) -> var_liquidity_412 {
            /// @src 1:4591,5819
            /// @src 1:4649,4663
            let zero_t_uint256_15 := zero_value_for_split_t_uint256()
            var_liquidity_412 := zero_t_uint256_15

            var_liquidity_412 := modifier_lock_410(var_liquidity_412, var_to_406)
        }

        function fun_mint_554_inner(_20, var_to_406) -> var_liquidity_412 {
            /// @src 1:4591,5819
            var_liquidity_412 := _20

            /// @src 1:4717,4728
            let expr_418_functionIdentifier := 104
            /// @src 1:4717,4730
            let expr_419_component_1, expr_419_component_2, expr_419_component_3 := fun_getReserves_104()
            /// @src 1:4675,4730
            let var__reserve0_415 := expr_419_component_1
            let var__reserve1_417 := expr_419_component_2
            /// @src 1:4778,4784
            let _21 := read_from_storage_split_offset_0_t_address(0x06)
            let expr_424 := _21
            /// @src 1:4771,4785
            let expr_425_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_424)
            /// @src 1:4771,4795
            let expr_426_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_425_address)
            let expr_426_functionSelector := 0x70a08231
            /// @src 1:4804,4808
            let expr_429_address := address()
            /// @src 1:4796,4809
            let expr_430 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_429_address)
            /// @src 1:4771,4810
            if iszero(extcodesize(expr_426_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _22 := allocate_unbounded()
            mstore(_22, shift_left_224(expr_426_functionSelector))
            let _23 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_22, 4) , expr_430)

            let _24 := staticcall(gas(), expr_426_address,  _22, sub(_23, _22), _22, 32)

            if iszero(_24) { revert_forward_1() }

            let expr_431
            if _24 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_22, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_431 :=  abi_decode_tuple_t_uint256_fromMemory(_22, add(_22, returndatasize()))
            }
            /// @src 1:4755,4810
            let var_balance0_422 := expr_431
            /// @src 1:4843,4849
            let _25 := read_from_storage_split_offset_0_t_address(0x07)
            let expr_436 := _25
            /// @src 1:4836,4850
            let expr_437_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_436)
            /// @src 1:4836,4860
            let expr_438_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_437_address)
            let expr_438_functionSelector := 0x70a08231
            /// @src 1:4869,4873
            let expr_441_address := address()
            /// @src 1:4861,4874
            let expr_442 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_441_address)
            /// @src 1:4836,4875
            if iszero(extcodesize(expr_438_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _26 := allocate_unbounded()
            mstore(_26, shift_left_224(expr_438_functionSelector))
            let _27 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_26, 4) , expr_442)

            let _28 := staticcall(gas(), expr_438_address,  _26, sub(_27, _26), _26, 32)

            if iszero(_28) { revert_forward_1() }

            let expr_443
            if _28 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_26, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_443 :=  abi_decode_tuple_t_uint256_fromMemory(_26, add(_26, returndatasize()))
            }
            /// @src 1:4820,4875
            let var_balance1_434 := expr_443
            /// @src 1:4900,4908
            let _29 := var_balance0_422
            let expr_447 := _29
            /// @src 1:4900,4912
            let expr_448_self := expr_447
            let expr_448_functionIdentifier := 2010
            /// @src 1:4913,4922
            let _30 := var__reserve0_415
            let expr_449 := _30
            /// @src 1:4900,4923
            let _31 := convert_t_uint112_to_t_uint256(expr_449)
            let expr_450 := fun_sub_2010(expr_448_self, _31)
            /// @src 1:4885,4923
            let var_amount0_446 := expr_450
            /// @src 1:4948,4956
            let _32 := var_balance1_434
            let expr_454 := _32
            /// @src 1:4948,4960
            let expr_455_self := expr_454
            let expr_455_functionIdentifier := 2010
            /// @src 1:4961,4970
            let _33 := var__reserve1_417
            let expr_456 := _33
            /// @src 1:4948,4971
            let _34 := convert_t_uint112_to_t_uint256(expr_456)
            let expr_457 := fun_sub_2010(expr_455_self, _34)
            /// @src 1:4933,4971
            let var_amount1_453 := expr_457
            /// @src 1:4995,5003
            let expr_461_functionIdentifier := 404
            /// @src 1:5004,5013
            let _35 := var__reserve0_415
            let expr_462 := _35
            /// @src 1:5015,5024
            let _36 := var__reserve1_417
            let expr_463 := _36
            /// @src 1:4995,5025
            let expr_464 := fun__mintFee_404(expr_462, expr_463)
            /// @src 1:4982,5025
            let var_feeOn_460 := expr_464
            /// @src 1:5055,5066
            let _37 := read_from_storage_split_offset_0_t_uint256(0x00)
            let expr_468 := _37
            /// @src 1:5035,5066
            let var__totalSupply_467 := expr_468
            /// @src 1:5158,5170
            let _38 := var__totalSupply_467
            let expr_470 := _38
            /// @src 1:5174,5175
            let expr_471 := 0x00
            /// @src 1:5158,5175
            let expr_472 := eq(cleanup_t_uint256(expr_470), convert_t_rational_0_by_1_to_t_uint256(expr_471))
            /// @src 1:5154,5501
            switch expr_472
            case 0 {
                /// @src 1:5404,5408
                let expr_496_address := linkersymbol("libraries/Math.sol:Math")
                /// @src 1:5404,5412
                let expr_497_functionIdentifier := 1747
                /// @src 1:5413,5420
                let _39 := var_amount0_446
                let expr_498 := _39
                /// @src 1:5413,5424
                let expr_499_self := expr_498
                let expr_499_functionIdentifier := 2038
                /// @src 1:5425,5437
                let _40 := var__totalSupply_467
                let expr_500 := _40
                /// @src 1:5413,5438
                let expr_501 := fun_mul_2038(expr_499_self, expr_500)
                /// @src 1:5441,5450
                let _41 := var__reserve0_415
                let expr_502 := _41
                /// @src 1:5413,5450
                let expr_503 := checked_div_t_uint256(expr_501, convert_t_uint112_to_t_uint256(expr_502))

                /// @src 1:5452,5459
                let _42 := var_amount1_453
                let expr_504 := _42
                /// @src 1:5452,5463
                let expr_505_self := expr_504
                let expr_505_functionIdentifier := 2038
                /// @src 1:5464,5476
                let _43 := var__totalSupply_467
                let expr_506 := _43
                /// @src 1:5452,5477
                let expr_507 := fun_mul_2038(expr_505_self, expr_506)
                /// @src 1:5480,5489
                let _44 := var__reserve1_417
                let expr_508 := _44
                /// @src 1:5452,5489
                let expr_509 := checked_div_t_uint256(expr_507, convert_t_uint112_to_t_uint256(expr_508))

                /// @src 1:5404,5490
                let expr_510 := fun_min_1747(expr_503, expr_509)
                /// @src 1:5392,5490
                var_liquidity_412 := expr_510
                let expr_511 := expr_510
                /// @src 1:5154,5501
            }
            default {
                /// @src 1:5203,5207
                let expr_474_address := linkersymbol("libraries/Math.sol:Math")
                /// @src 1:5203,5212
                let expr_475_functionIdentifier := 1801
                /// @src 1:5213,5220
                let _45 := var_amount0_446
                let expr_476 := _45
                /// @src 1:5213,5224
                let expr_477_self := expr_476
                let expr_477_functionIdentifier := 2038
                /// @src 1:5225,5232
                let _46 := var_amount1_453
                let expr_478 := _46
                /// @src 1:5213,5233
                let expr_479 := fun_mul_2038(expr_477_self, expr_478)
                /// @src 1:5203,5234
                let expr_480 := fun_sqrt_1801(expr_479)
                /// @src 1:5203,5238
                let expr_481_self := expr_480
                let expr_481_functionIdentifier := 2010
                /// @src 1:5239,5256
                let expr_482 := constant_MINIMUM_LIQUIDITY_24()
                /// @src 1:5203,5257
                let expr_483 := fun_sub_2010(expr_481_self, expr_482)
                /// @src 1:5191,5257
                var_liquidity_412 := expr_483
                let expr_484 := expr_483
                /// @src 1:5270,5275
                let expr_486_functionIdentifier := 1165
                /// @src 1:5284,5285
                let expr_489 := 0x00
                /// @src 1:5276,5286
                let expr_490 := convert_t_rational_0_by_1_to_t_address(expr_489)
                /// @src 1:5288,5305
                let expr_491 := constant_MINIMUM_LIQUIDITY_24()
                fun__mint_1165(expr_490, expr_491)
                /// @src 1:5154,5501
            }
            /// @src 1:5518,5527
            let _47 := var_liquidity_412
            let expr_516 := _47
            /// @src 1:5530,5531
            let expr_517 := 0x00
            /// @src 1:5518,5531
            let expr_518 := gt(cleanup_t_uint256(expr_516), convert_t_rational_0_by_1_to_t_uint256(expr_517))
            /// @src 1:5510,5576
            require_helper_t_stringliteral_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e(expr_518)
            /// @src 1:5586,5591
            let expr_522_functionIdentifier := 1165
            /// @src 1:5592,5594
            let _48 := var_to_406
            let expr_523 := _48
            /// @src 1:5596,5605
            let _49 := var_liquidity_412
            let expr_524 := _49
            fun__mint_1165(expr_523, expr_524)
            /// @src 1:5617,5624
            let expr_527_functionIdentifier := 296
            /// @src 1:5625,5633
            let _50 := var_balance0_422
            let expr_528 := _50
            /// @src 1:5635,5643
            let _51 := var_balance1_434
            let expr_529 := _51
            /// @src 1:5645,5654
            let _52 := var__reserve0_415
            let expr_530 := _52
            /// @src 1:5656,5665
            let _53 := var__reserve1_417
            let expr_531 := _53
            fun__update_296(expr_528, expr_529, expr_530, expr_531)
            /// @src 1:5680,5685
            let _54 := var_feeOn_460
            let expr_534 := _54
            /// @src 1:5676,5723
            if expr_534 {
                /// @src 1:5700,5708
                let _55 := read_from_storage_split_offset_0_t_uint112(0x08)
                let expr_538 := _55
                /// @src 1:5695,5709
                let expr_539 := convert_t_uint112_to_t_uint256(expr_538)
                /// @src 1:5695,5713
                let expr_540_self := expr_539
                let expr_540_functionIdentifier := 2038
                /// @src 1:5714,5722
                let _56 := read_from_storage_split_offset_14_t_uint112(0x08)
                let expr_541 := _56
                /// @src 1:5695,5723
                let _57 := convert_t_uint112_to_t_uint256(expr_541)
                let expr_542 := fun_mul_2038(expr_540_self, _57)
                /// @src 1:5687,5723
                update_storage_value_offset_0t_uint256_to_t_uint256(0x0b, expr_542)
                let expr_543 := expr_542
                /// @src 1:5676,5723
            }
            /// @src 1:5783,5793
            let expr_548 := caller()
            /// @src 1:5795,5802
            let _58 := var_amount0_446
            let expr_549 := _58
            /// @src 1:5804,5811
            let _59 := var_amount1_453
            let expr_550 := _59
            /// @src 1:5778,5812
            let _60 := 0x4c209b5fc8ad50758f13e2e1088ba56a560dff690a1c6fef26394f4c03821c4f
            {
                let _61 := allocate_unbounded()
                let _62 := abi_encode_tuple_t_uint256_t_uint256__to_t_uint256_t_uint256__fromStack(_61 , expr_549, expr_550)
                log2(_61, sub(_62, _61) , _60, expr_548)
            }
        }

        function fun_mul_2038(var_x_2012, var_y_2014) -> var_z_2017 {
            /// @src 8:419,559
            /// @src 8:471,477
            let zero_t_uint256_454 := zero_value_for_split_t_uint256()
            var_z_2017 := zero_t_uint256_454

            /// @src 8:497,498
            let _455 := var_y_2014
            let expr_2020 := _455
            /// @src 8:502,503
            let expr_2021 := 0x00
            /// @src 8:497,503
            let expr_2022 := eq(cleanup_t_uint256(expr_2020), convert_t_rational_0_by_1_to_t_uint256(expr_2021))
            /// @src 8:497,527
            let expr_2033 := expr_2022
            if iszero(expr_2033) {
                /// @src 8:512,513
                let _456 := var_x_2012
                let expr_2024 := _456
                /// @src 8:516,517
                let _457 := var_y_2014
                let expr_2025 := _457
                /// @src 8:512,517
                let expr_2026 := checked_mul_t_uint256(expr_2024, expr_2025)

                /// @src 8:508,517
                var_z_2017 := expr_2026
                let expr_2027 := expr_2026
                /// @src 8:507,518
                let expr_2028 := expr_2027
                /// @src 8:521,522
                let _458 := var_y_2014
                let expr_2029 := _458
                /// @src 8:507,522
                let expr_2030 := checked_div_t_uint256(expr_2028, expr_2029)

                /// @src 8:526,527
                let _459 := var_x_2012
                let expr_2031 := _459
                /// @src 8:507,527
                let expr_2032 := eq(cleanup_t_uint256(expr_2030), cleanup_t_uint256(expr_2031))
                /// @src 8:497,527
                expr_2033 := expr_2032
            }
            /// @src 8:489,552
            require_helper_t_stringliteral_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd(expr_2033)

        }

        function fun_permit_1437(var_owner_1359, var_spender_1361, var_value_1363, var_deadline_1365, var_v_1367, var_r_1369, var_s_1371) {
            /// @src 0:2889,3560

            /// @src 0:3031,3039
            let _395 := var_deadline_1365
            let expr_1376 := _395
            /// @src 0:3043,3058
            let expr_1378 := timestamp()
            /// @src 0:3031,3058
            let expr_1379 := iszero(lt(cleanup_t_uint256(expr_1376), cleanup_t_uint256(expr_1378)))
            /// @src 0:3023,3081
            require_helper_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87(expr_1379)
            /// @src 0:3193,3209
            let _396 := read_from_storage_split_offset_0_t_bytes32(0x03)
            let expr_1389 := _396
            /// @src 0:3248,3263
            let expr_1393 := constant_PERMIT_TYPEHASH_1089()
            /// @src 0:3265,3270
            let _397 := var_owner_1359
            let expr_1394 := _397
            /// @src 0:3272,3279
            let _398 := var_spender_1361
            let expr_1395 := _398
            /// @src 0:3281,3286
            let _399 := var_value_1363
            let expr_1396 := _399
            /// @src 0:3288,3294
            let _400 := 0x04
            let expr_1397 := _400
            /// @src 0:3295,3300
            let _401 := var_owner_1359
            let expr_1398 := _401
            /// @src 0:3288,3301
            let _402 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1397,expr_1398)
            /// @src 0:3288,3303
            let _404 := read_from_storage_split_offset_0_t_uint256(_402)
            let _403 := increment_t_uint256(_404)
            update_storage_value_offset_0t_uint256_to_t_uint256(_402, _403)
            let expr_1400 := _404
            /// @src 0:3305,3313
            let _405 := var_deadline_1365
            let expr_1401 := _405
            /// @src 0:3237,3314

            let expr_1402_mpos := allocate_unbounded()
            let _406 := add(expr_1402_mpos, 0x20)

            let _407 := abi_encode_tuple_t_bytes32_t_address_t_address_t_uint256_t_uint256_t_uint256__to_t_bytes32_t_address_t_address_t_uint256_t_uint256_t_uint256__fromStack(_406, expr_1393, expr_1394, expr_1395, expr_1396, expr_1400, expr_1401)
            mstore(expr_1402_mpos, sub(_407, add(expr_1402_mpos, 0x20)))
            finalize_allocation(expr_1402_mpos, sub(_407, expr_1402_mpos))
            /// @src 0:3227,3315
            let expr_1403 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_1402_mpos), array_length_t_bytes_memory_ptr(expr_1402_mpos))
            /// @src 0:3131,3329

            let expr_1404_mpos := allocate_unbounded()
            let _408 := add(expr_1404_mpos, 0x20)

            let _409 := abi_encode_tuple_packed_t_stringliteral_301a50b291d33ce1e8e9064e3f6a6c51d902ec22892b50d58abf6357c6a45541_t_bytes32_t_bytes32__to_t_string_memory_ptr_t_bytes32_t_bytes32__nonPadded_inplace_fromStack(_408, expr_1389, expr_1403)
            mstore(expr_1404_mpos, sub(_409, add(expr_1404_mpos, 0x20)))
            finalize_allocation(expr_1404_mpos, sub(_409, expr_1404_mpos))
            /// @src 0:3108,3339
            let expr_1405 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_1404_mpos), array_length_t_bytes_memory_ptr(expr_1404_mpos))
            /// @src 0:3091,3339
            let var_digest_1384 := expr_1405
            /// @src 0:3386,3392
            let _410 := var_digest_1384
            let expr_1410 := _410
            /// @src 0:3394,3395
            let _411 := var_v_1367
            let expr_1411 := _411
            /// @src 0:3397,3398
            let _412 := var_r_1369
            let expr_1412 := _412
            /// @src 0:3400,3401
            let _413 := var_s_1371
            let expr_1413 := _413
            /// @src 0:3376,3402

            let _414 := allocate_unbounded()
            let _415 := abi_encode_tuple_t_bytes32_t_uint8_t_bytes32_t_bytes32__to_t_bytes32_t_uint8_t_bytes32_t_bytes32__fromStack(_414 , expr_1410, expr_1411, expr_1412, expr_1413)

            mstore(0, 0)

            let _416 := staticcall(gas(), 1 , _414, sub(_415, _414), 0, 32)
            if iszero(_416) { revert_forward_1() }
            let expr_1414 := shift_left_0(mload(0))
            /// @src 0:3349,3402
            let var_recoveredAddress_1408 := expr_1414
            /// @src 0:3420,3436
            let _417 := var_recoveredAddress_1408
            let expr_1417 := _417
            /// @src 0:3448,3449
            let expr_1420 := 0x00
            /// @src 0:3440,3450
            let expr_1421 := convert_t_rational_0_by_1_to_t_address(expr_1420)
            /// @src 0:3420,3450
            let expr_1422 := iszero(eq(cleanup_t_address(expr_1417), cleanup_t_address(expr_1421)))
            /// @src 0:3420,3479
            let expr_1426 := expr_1422
            if expr_1426 {
                /// @src 0:3454,3470
                let _418 := var_recoveredAddress_1408
                let expr_1423 := _418
                /// @src 0:3474,3479
                let _419 := var_owner_1359
                let expr_1424 := _419
                /// @src 0:3454,3479
                let expr_1425 := eq(cleanup_t_address(expr_1423), cleanup_t_address(expr_1424))
                /// @src 0:3420,3479
                expr_1426 := expr_1425
            }
            /// @src 0:3412,3512
            require_helper_t_stringliteral_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56(expr_1426)
            /// @src 0:3522,3530
            let expr_1430_functionIdentifier := 1224
            /// @src 0:3531,3536
            let _420 := var_owner_1359
            let expr_1431 := _420
            /// @src 0:3538,3545
            let _421 := var_spender_1361
            let expr_1432 := _421
            /// @src 0:3547,3552
            let _422 := var_value_1363
            let expr_1433 := _422
            fun__approve_1224(expr_1431, expr_1432, expr_1433)

        }

        function fun_skim_1015(var_to_966) {
            /// @src 1:9406,9744

            modifier_lock_970(var_to_966)
        }

        function fun_skim_1015_inner(var_to_966) {
            /// @src 1:9406,9744

            /// @src 1:9483,9489
            let _271 := read_from_storage_split_offset_0_t_address(0x06)
            let expr_974 := _271
            /// @src 1:9465,9489
            let var__token0_973 := expr_974
            /// @src 1:9532,9538
            let _272 := read_from_storage_split_offset_0_t_address(0x07)
            let expr_978 := _272
            /// @src 1:9514,9538
            let var__token1_977 := expr_978
            /// @src 1:9563,9576
            let expr_980_functionIdentifier := 147
            /// @src 1:9577,9584
            let _273 := var__token0_973
            let expr_981 := _273
            /// @src 1:9586,9588
            let _274 := var_to_966
            let expr_982 := _274
            /// @src 1:9597,9604
            let _275 := var__token0_973
            let expr_984 := _275
            /// @src 1:9590,9605
            let expr_985_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_984)
            /// @src 1:9590,9615
            let expr_986_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_985_address)
            let expr_986_functionSelector := 0x70a08231
            /// @src 1:9624,9628
            let expr_989_address := address()
            /// @src 1:9616,9629
            let expr_990 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_989_address)
            /// @src 1:9590,9630
            if iszero(extcodesize(expr_986_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _276 := allocate_unbounded()
            mstore(_276, shift_left_224(expr_986_functionSelector))
            let _277 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_276, 4) , expr_990)

            let _278 := staticcall(gas(), expr_986_address,  _276, sub(_277, _276), _276, 32)

            if iszero(_278) { revert_forward_1() }

            let expr_991
            if _278 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_276, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_991 :=  abi_decode_tuple_t_uint256_fromMemory(_276, add(_276, returndatasize()))
            }
            /// @src 1:9590,9634
            let expr_992_self := expr_991
            let expr_992_functionIdentifier := 2010
            /// @src 1:9635,9643
            let _279 := read_from_storage_split_offset_0_t_uint112(0x08)
            let expr_993 := _279
            /// @src 1:9590,9644
            let _280 := convert_t_uint112_to_t_uint256(expr_993)
            let expr_994 := fun_sub_2010(expr_992_self, _280)
            fun__safeTransfer_147(expr_981, expr_982, expr_994)
            /// @src 1:9655,9668
            let expr_997_functionIdentifier := 147
            /// @src 1:9669,9676
            let _281 := var__token1_977
            let expr_998 := _281
            /// @src 1:9678,9680
            let _282 := var_to_966
            let expr_999 := _282
            /// @src 1:9689,9696
            let _283 := var__token1_977
            let expr_1001 := _283
            /// @src 1:9682,9697
            let expr_1002_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_1001)
            /// @src 1:9682,9707
            let expr_1003_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_1002_address)
            let expr_1003_functionSelector := 0x70a08231
            /// @src 1:9716,9720
            let expr_1006_address := address()
            /// @src 1:9708,9721
            let expr_1007 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_1006_address)
            /// @src 1:9682,9722
            if iszero(extcodesize(expr_1003_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _284 := allocate_unbounded()
            mstore(_284, shift_left_224(expr_1003_functionSelector))
            let _285 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_284, 4) , expr_1007)

            let _286 := staticcall(gas(), expr_1003_address,  _284, sub(_285, _284), _284, 32)

            if iszero(_286) { revert_forward_1() }

            let expr_1008
            if _286 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_284, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_1008 :=  abi_decode_tuple_t_uint256_fromMemory(_284, add(_284, returndatasize()))
            }
            /// @src 1:9682,9726
            let expr_1009_self := expr_1008
            let expr_1009_functionIdentifier := 2010
            /// @src 1:9727,9735
            let _287 := read_from_storage_split_offset_14_t_uint112(0x08)
            let expr_1010 := _287
            /// @src 1:9682,9736
            let _288 := convert_t_uint112_to_t_uint256(expr_1010)
            let expr_1011 := fun_sub_2010(expr_1009_self, _288)
            fun__safeTransfer_147(expr_998, expr_999, expr_1011)

        }

        function fun_sqrt_1801(var_y_1749) -> var_z_1752 {
            /// @src 7:306,598
            /// @src 7:351,357
            let zero_t_uint256_428 := zero_value_for_split_t_uint256()
            var_z_1752 := zero_t_uint256_428

            /// @src 7:373,374
            let _429 := var_y_1749
            let expr_1754 := _429
            /// @src 7:377,378
            let expr_1755 := 0x03
            /// @src 7:373,378
            let expr_1756 := gt(cleanup_t_uint256(expr_1754), convert_t_rational_3_by_1_to_t_uint256(expr_1755))
            /// @src 7:369,592
            switch expr_1756
            case 0 {
                /// @src 7:554,555
                let _430 := var_y_1749
                let expr_1790 := _430
                /// @src 7:559,560
                let expr_1791 := 0x00
                /// @src 7:554,560
                let expr_1792 := iszero(eq(cleanup_t_uint256(expr_1790), convert_t_rational_0_by_1_to_t_uint256(expr_1791)))
                /// @src 7:550,592
                if expr_1792 {
                    /// @src 7:580,581
                    let expr_1794 := 0x01
                    /// @src 7:576,581
                    let _431 := convert_t_rational_1_by_1_to_t_uint256(expr_1794)
                    var_z_1752 := _431
                    let expr_1795 := _431
                    /// @src 7:550,592
                }
                /// @src 7:369,592
            }
            default {
                /// @src 7:398,399
                let _432 := var_y_1749
                let expr_1758 := _432
                /// @src 7:394,399
                var_z_1752 := expr_1758
                let expr_1759 := expr_1758
                /// @src 7:422,423
                let _433 := var_y_1749
                let expr_1763 := _433
                /// @src 7:426,427
                let expr_1764 := 0x02
                /// @src 7:422,427
                let expr_1765 := checked_div_t_uint256(expr_1763, convert_t_rational_2_by_1_to_t_uint256(expr_1764))

                /// @src 7:430,431
                let expr_1766 := 0x01
                /// @src 7:422,431
                let expr_1767 := checked_add_t_uint256(expr_1765, convert_t_rational_1_by_1_to_t_uint256(expr_1766))

                /// @src 7:413,431
                let var_x_1762 := expr_1767
                /// @src 7:445,534
                for {
                    } 1 {
                }
                {
                    /// @src 7:452,453
                    let _434 := var_x_1762
                    let expr_1769 := _434
                    /// @src 7:456,457
                    let _435 := var_z_1752
                    let expr_1770 := _435
                    /// @src 7:452,457
                    let expr_1771 := lt(cleanup_t_uint256(expr_1769), cleanup_t_uint256(expr_1770))
                    if iszero(expr_1771) { break }
                    /// @src 7:481,482
                    let _436 := var_x_1762
                    let expr_1773 := _436
                    /// @src 7:477,482
                    var_z_1752 := expr_1773
                    let expr_1774 := expr_1773
                    /// @src 7:505,506
                    let _437 := var_y_1749
                    let expr_1777 := _437
                    /// @src 7:509,510
                    let _438 := var_x_1762
                    let expr_1778 := _438
                    /// @src 7:505,510
                    let expr_1779 := checked_div_t_uint256(expr_1777, expr_1778)

                    /// @src 7:513,514
                    let _439 := var_x_1762
                    let expr_1780 := _439
                    /// @src 7:505,514
                    let expr_1781 := checked_add_t_uint256(expr_1779, expr_1780)

                    /// @src 7:504,515
                    let expr_1782 := expr_1781
                    /// @src 7:518,519
                    let expr_1783 := 0x02
                    /// @src 7:504,519
                    let expr_1784 := checked_div_t_uint256(expr_1782, convert_t_rational_2_by_1_to_t_uint256(expr_1783))

                    /// @src 7:500,519
                    var_x_1762 := expr_1784
                    let expr_1785 := expr_1784
                }
                /// @src 7:369,592
            }

        }

        function fun_sub_2010(var_x_1990, var_y_1992) -> var_z_1995 {
            /// @src 8:286,413
            /// @src 8:338,344
            let zero_t_uint256_450 := zero_value_for_split_t_uint256()
            var_z_1995 := zero_t_uint256_450

            /// @src 8:369,370
            let _451 := var_x_1990
            let expr_1999 := _451
            /// @src 8:373,374
            let _452 := var_y_1992
            let expr_2000 := _452
            /// @src 8:369,374
            let expr_2001 := checked_sub_t_uint256(expr_1999, expr_2000)

            /// @src 8:365,374
            var_z_1995 := expr_2001
            let expr_2002 := expr_2001
            /// @src 8:364,375
            let expr_2003 := expr_2002
            /// @src 8:379,380
            let _453 := var_x_1990
            let expr_2004 := _453
            /// @src 8:364,380
            let expr_2005 := iszero(gt(cleanup_t_uint256(expr_2003), cleanup_t_uint256(expr_2004)))
            /// @src 8:356,406
            require_helper_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590(expr_2005)

        }

        function fun_swap_964(var_amount0Out_728, var_amount1Out_730, var_to_732, var_data_734_offset, var_data_734_length) {
            /// @src 1:7490,9360

            modifier_lock_738(var_amount0Out_728, var_amount1Out_730, var_to_732, var_data_734_offset, var_data_734_length)
        }

        function fun_swap_964_inner(var_amount0Out_728, var_amount1Out_730, var_to_732, var_data_734_offset, var_data_734_length) {
            /// @src 1:7490,9360

            /// @src 1:7612,7622
            let _190 := var_amount0Out_728
            let expr_741 := _190
            /// @src 1:7625,7626
            let expr_742 := 0x00
            /// @src 1:7612,7626
            let expr_743 := gt(cleanup_t_uint256(expr_741), convert_t_rational_0_by_1_to_t_uint256(expr_742))
            /// @src 1:7612,7644
            let expr_747 := expr_743
            if iszero(expr_747) {
                /// @src 1:7630,7640
                let _191 := var_amount1Out_730
                let expr_744 := _191
                /// @src 1:7643,7644
                let expr_745 := 0x00
                /// @src 1:7630,7644
                let expr_746 := gt(cleanup_t_uint256(expr_744), convert_t_rational_0_by_1_to_t_uint256(expr_745))
                /// @src 1:7612,7644
                expr_747 := expr_746
            }
            /// @src 1:7604,7686
            require_helper_t_stringliteral_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83(expr_747)
            /// @src 1:7738,7749
            let expr_755_functionIdentifier := 104
            /// @src 1:7738,7751
            let expr_756_component_1, expr_756_component_2, expr_756_component_3 := fun_getReserves_104()
            /// @src 1:7696,7751
            let var__reserve0_752 := expr_756_component_1
            let var__reserve1_754 := expr_756_component_2
            /// @src 1:7784,7794
            let _192 := var_amount0Out_728
            let expr_759 := _192
            /// @src 1:7797,7806
            let _193 := var__reserve0_752
            let expr_760 := _193
            /// @src 1:7784,7806
            let expr_761 := lt(cleanup_t_uint256(expr_759), convert_t_uint112_to_t_uint256(expr_760))
            /// @src 1:7784,7832
            let expr_765 := expr_761
            if expr_765 {
                /// @src 1:7810,7820
                let _194 := var_amount1Out_730
                let expr_762 := _194
                /// @src 1:7823,7832
                let _195 := var__reserve1_754
                let expr_763 := _195
                /// @src 1:7810,7832
                let expr_764 := lt(cleanup_t_uint256(expr_762), convert_t_uint112_to_t_uint256(expr_763))
                /// @src 1:7784,7832
                expr_765 := expr_764
            }
            /// @src 1:7776,7870
            require_helper_t_stringliteral_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f(expr_765)
            /// @src 1:7881,7894
            let var_balance0_770
            let zero_t_uint256_196 := zero_value_for_split_t_uint256()
            var_balance0_770 := zero_t_uint256_196
            /// @src 1:7904,7917
            let var_balance1_773
            let zero_t_uint256_197 := zero_value_for_split_t_uint256()
            var_balance1_773 := zero_t_uint256_197
            /// @src 1:8010,8016
            let _198 := read_from_storage_split_offset_0_t_address(0x06)
            let expr_777 := _198
            /// @src 1:7992,8016
            let var__token0_776 := expr_777
            /// @src 1:8044,8050
            let _199 := read_from_storage_split_offset_0_t_address(0x07)
            let expr_781 := _199
            /// @src 1:8026,8050
            let var__token1_780 := expr_781
            /// @src 1:8068,8070
            let _200 := var_to_732
            let expr_784 := _200
            /// @src 1:8074,8081
            let _201 := var__token0_776
            let expr_785 := _201
            /// @src 1:8068,8081
            let expr_786 := iszero(eq(cleanup_t_address(expr_784), cleanup_t_address(expr_785)))
            /// @src 1:8068,8098
            let expr_790 := expr_786
            if expr_790 {
                /// @src 1:8085,8087
                let _202 := var_to_732
                let expr_787 := _202
                /// @src 1:8091,8098
                let _203 := var__token1_780
                let expr_788 := _203
                /// @src 1:8085,8098
                let expr_789 := iszero(eq(cleanup_t_address(expr_787), cleanup_t_address(expr_788)))
                /// @src 1:8068,8098
                expr_790 := expr_789
            }
            /// @src 1:8060,8124
            require_helper_t_stringliteral_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3(expr_790)
            /// @src 1:8138,8148
            let _204 := var_amount0Out_728
            let expr_794 := _204
            /// @src 1:8151,8152
            let expr_795 := 0x00
            /// @src 1:8138,8152
            let expr_796 := gt(cleanup_t_uint256(expr_794), convert_t_rational_0_by_1_to_t_uint256(expr_795))
            /// @src 1:8134,8192
            if expr_796 {
                /// @src 1:8154,8167
                let expr_797_functionIdentifier := 147
                /// @src 1:8168,8175
                let _205 := var__token0_776
                let expr_798 := _205
                /// @src 1:8177,8179
                let _206 := var_to_732
                let expr_799 := _206
                /// @src 1:8181,8191
                let _207 := var_amount0Out_728
                let expr_800 := _207
                fun__safeTransfer_147(expr_798, expr_799, expr_800)
                /// @src 1:8134,8192
            }
            /// @src 1:8240,8250
            let _208 := var_amount1Out_730
            let expr_804 := _208
            /// @src 1:8253,8254
            let expr_805 := 0x00
            /// @src 1:8240,8254
            let expr_806 := gt(cleanup_t_uint256(expr_804), convert_t_rational_0_by_1_to_t_uint256(expr_805))
            /// @src 1:8236,8294
            if expr_806 {
                /// @src 1:8256,8269
                let expr_807_functionIdentifier := 147
                /// @src 1:8270,8277
                let _209 := var__token1_780
                let expr_808 := _209
                /// @src 1:8279,8281
                let _210 := var_to_732
                let expr_809 := _210
                /// @src 1:8283,8293
                let _211 := var_amount1Out_730
                let expr_810 := _211
                fun__safeTransfer_147(expr_808, expr_809, expr_810)
                /// @src 1:8236,8294
            }
            /// @src 1:8342,8346
            let _212_offset := var_data_734_offset
            let _212_length := var_data_734_length
            let expr_814_offset := _212_offset
            let expr_814_length := _212_length
            /// @src 1:8342,8353
            let expr_815 := array_length_t_bytes_calldata_ptr(expr_814_offset, expr_814_length)
            /// @src 1:8356,8357
            let expr_816 := 0x00
            /// @src 1:8342,8357
            let expr_817 := gt(cleanup_t_uint256(expr_815), convert_t_rational_0_by_1_to_t_uint256(expr_816))
            /// @src 1:8338,8435
            if expr_817 {
                /// @src 1:8376,8378
                let _213 := var_to_732
                let expr_819 := _213
                /// @src 1:8359,8379
                let expr_820_address := convert_t_address_to_t_contract$_IUniswapV2Callee_$1536(expr_819)
                /// @src 1:8359,8393
                let expr_821_address := convert_t_contract$_IUniswapV2Callee_$1536_to_t_address(expr_820_address)
                let expr_821_functionSelector := 0x10d1e85c
                /// @src 1:8394,8404
                let expr_823 := caller()
                /// @src 1:8406,8416
                let _214 := var_amount0Out_728
                let expr_824 := _214
                /// @src 1:8418,8428
                let _215 := var_amount1Out_730
                let expr_825 := _215
                /// @src 1:8430,8434
                let _216_offset := var_data_734_offset
                let _216_length := var_data_734_length
                let expr_826_offset := _216_offset
                let expr_826_length := _216_length
                /// @src 1:8359,8435
                if iszero(extcodesize(expr_821_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                // storage for arguments and returned data
                let _217 := allocate_unbounded()
                mstore(_217, shift_left_224(expr_821_functionSelector))
                let _218 := abi_encode_tuple_t_address_t_uint256_t_uint256_t_bytes_calldata_ptr__to_t_address_t_uint256_t_uint256_t_bytes_memory_ptr__fromStack(add(_217, 4) , expr_823, expr_824, expr_825, expr_826_offset, expr_826_length)

                let _219 := call(gas(), expr_821_address,  0,  _217, sub(_218, _217), _217, 0)

                if iszero(_219) { revert_forward_1() }

                if _219 {

                    // update freeMemoryPointer according to dynamic return size
                    finalize_allocation(_217, returndatasize())

                    // decode return parameters from external try-call into retVars
                    abi_decode_tuple__fromMemory(_217, add(_217, returndatasize()))
                }
                /// @src 1:8338,8435
            }
            /// @src 1:8463,8470
            let _220 := var__token0_776
            let expr_832 := _220
            /// @src 1:8456,8471
            let expr_833_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_832)
            /// @src 1:8456,8481
            let expr_834_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_833_address)
            let expr_834_functionSelector := 0x70a08231
            /// @src 1:8490,8494
            let expr_837_address := address()
            /// @src 1:8482,8495
            let expr_838 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_837_address)
            /// @src 1:8456,8496
            if iszero(extcodesize(expr_834_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _221 := allocate_unbounded()
            mstore(_221, shift_left_224(expr_834_functionSelector))
            let _222 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_221, 4) , expr_838)

            let _223 := staticcall(gas(), expr_834_address,  _221, sub(_222, _221), _221, 32)

            if iszero(_223) { revert_forward_1() }

            let expr_839
            if _223 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_221, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_839 :=  abi_decode_tuple_t_uint256_fromMemory(_221, add(_221, returndatasize()))
            }
            /// @src 1:8445,8496
            var_balance0_770 := expr_839
            let expr_840 := expr_839
            /// @src 1:8524,8531
            let _224 := var__token1_780
            let expr_844 := _224
            /// @src 1:8517,8532
            let expr_845_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_844)
            /// @src 1:8517,8542
            let expr_846_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_845_address)
            let expr_846_functionSelector := 0x70a08231
            /// @src 1:8551,8555
            let expr_849_address := address()
            /// @src 1:8543,8556
            let expr_850 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_849_address)
            /// @src 1:8517,8557
            if iszero(extcodesize(expr_846_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _225 := allocate_unbounded()
            mstore(_225, shift_left_224(expr_846_functionSelector))
            let _226 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_225, 4) , expr_850)

            let _227 := staticcall(gas(), expr_846_address,  _225, sub(_226, _225), _225, 32)

            if iszero(_227) { revert_forward_1() }

            let expr_851
            if _227 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_225, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_851 :=  abi_decode_tuple_t_uint256_fromMemory(_225, add(_225, returndatasize()))
            }
            /// @src 1:8506,8557
            var_balance1_773 := expr_851
            let expr_852 := expr_851
            /// @src 1:8594,8602
            let _228 := var_balance0_770
            let expr_857 := _228
            /// @src 1:8605,8614
            let _229 := var__reserve0_752
            let expr_858 := _229
            /// @src 1:8617,8627
            let _230 := var_amount0Out_728
            let expr_859 := _230
            /// @src 1:8605,8627
            let expr_860 := checked_sub_t_uint256(convert_t_uint112_to_t_uint256(expr_858), expr_859)

            /// @src 1:8594,8627
            let expr_861 := gt(cleanup_t_uint256(expr_857), cleanup_t_uint256(expr_860))
            /// @src 1:8594,8669
            let expr_869
            switch expr_861
            case 0 {
                /// @src 1:8668,8669
                let expr_868 := 0x00
                /// @src 1:8594,8669
                expr_869 := convert_t_rational_0_by_1_to_t_uint256(expr_868)
            }
            default {
                /// @src 1:8630,8638
                let _231 := var_balance0_770
                let expr_862 := _231
                /// @src 1:8642,8651
                let _232 := var__reserve0_752
                let expr_863 := _232
                /// @src 1:8654,8664
                let _233 := var_amount0Out_728
                let expr_864 := _233
                /// @src 1:8642,8664
                let expr_865 := checked_sub_t_uint256(convert_t_uint112_to_t_uint256(expr_863), expr_864)

                /// @src 1:8641,8665
                let expr_866 := expr_865
                /// @src 1:8630,8665
                let expr_867 := checked_sub_t_uint256(expr_862, expr_866)

                /// @src 1:8594,8669
                expr_869 := expr_867
            }
            /// @src 1:8577,8669
            let var_amount0In_856 := expr_869
            /// @src 1:8696,8704
            let _234 := var_balance1_773
            let expr_873 := _234
            /// @src 1:8707,8716
            let _235 := var__reserve1_754
            let expr_874 := _235
            /// @src 1:8719,8729
            let _236 := var_amount1Out_730
            let expr_875 := _236
            /// @src 1:8707,8729
            let expr_876 := checked_sub_t_uint256(convert_t_uint112_to_t_uint256(expr_874), expr_875)

            /// @src 1:8696,8729
            let expr_877 := gt(cleanup_t_uint256(expr_873), cleanup_t_uint256(expr_876))
            /// @src 1:8696,8771
            let expr_885
            switch expr_877
            case 0 {
                /// @src 1:8770,8771
                let expr_884 := 0x00
                /// @src 1:8696,8771
                expr_885 := convert_t_rational_0_by_1_to_t_uint256(expr_884)
            }
            default {
                /// @src 1:8732,8740
                let _237 := var_balance1_773
                let expr_878 := _237
                /// @src 1:8744,8753
                let _238 := var__reserve1_754
                let expr_879 := _238
                /// @src 1:8756,8766
                let _239 := var_amount1Out_730
                let expr_880 := _239
                /// @src 1:8744,8766
                let expr_881 := checked_sub_t_uint256(convert_t_uint112_to_t_uint256(expr_879), expr_880)

                /// @src 1:8743,8767
                let expr_882 := expr_881
                /// @src 1:8732,8767
                let expr_883 := checked_sub_t_uint256(expr_878, expr_882)

                /// @src 1:8696,8771
                expr_885 := expr_883
            }
            /// @src 1:8679,8771
            let var_amount1In_872 := expr_885
            /// @src 1:8789,8798
            let _240 := var_amount0In_856
            let expr_888 := _240
            /// @src 1:8801,8802
            let expr_889 := 0x00
            /// @src 1:8789,8802
            let expr_890 := gt(cleanup_t_uint256(expr_888), convert_t_rational_0_by_1_to_t_uint256(expr_889))
            /// @src 1:8789,8819
            let expr_894 := expr_890
            if iszero(expr_894) {
                /// @src 1:8806,8815
                let _241 := var_amount1In_872
                let expr_891 := _241
                /// @src 1:8818,8819
                let expr_892 := 0x00
                /// @src 1:8806,8819
                let expr_893 := gt(cleanup_t_uint256(expr_891), convert_t_rational_0_by_1_to_t_uint256(expr_892))
                /// @src 1:8789,8819
                expr_894 := expr_893
            }
            /// @src 1:8781,8860
            require_helper_t_stringliteral_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67(expr_894)
            /// @src 1:8968,8976
            let _242 := var_balance0_770
            let expr_900 := _242
            /// @src 1:8968,8980
            let expr_901_self := expr_900
            let expr_901_functionIdentifier := 2038
            /// @src 1:8981,8985
            let expr_902 := 0x03e8
            /// @src 1:8968,8986
            let _243 := convert_t_rational_1000_by_1_to_t_uint256(expr_902)
            let expr_903 := fun_mul_2038(expr_901_self, _243)
            /// @src 1:8968,8990
            let expr_904_self := expr_903
            let expr_904_functionIdentifier := 2010
            /// @src 1:8991,9000
            let _244 := var_amount0In_856
            let expr_905 := _244
            /// @src 1:8991,9004
            let expr_906_self := expr_905
            let expr_906_functionIdentifier := 2038
            /// @src 1:9005,9006
            let expr_907 := 0x03
            /// @src 1:8991,9007
            let _245 := convert_t_rational_3_by_1_to_t_uint256(expr_907)
            let expr_908 := fun_mul_2038(expr_906_self, _245)
            /// @src 1:8968,9008
            let expr_909 := fun_sub_2010(expr_904_self, expr_908)
            /// @src 1:8944,9008
            let var_balance0Adjusted_899 := expr_909
            /// @src 1:9042,9050
            let _246 := var_balance1_773
            let expr_913 := _246
            /// @src 1:9042,9054
            let expr_914_self := expr_913
            let expr_914_functionIdentifier := 2038
            /// @src 1:9055,9059
            let expr_915 := 0x03e8
            /// @src 1:9042,9060
            let _247 := convert_t_rational_1000_by_1_to_t_uint256(expr_915)
            let expr_916 := fun_mul_2038(expr_914_self, _247)
            /// @src 1:9042,9064
            let expr_917_self := expr_916
            let expr_917_functionIdentifier := 2010
            /// @src 1:9065,9074
            let _248 := var_amount1In_872
            let expr_918 := _248
            /// @src 1:9065,9078
            let expr_919_self := expr_918
            let expr_919_functionIdentifier := 2038
            /// @src 1:9079,9080
            let expr_920 := 0x03
            /// @src 1:9065,9081
            let _249 := convert_t_rational_3_by_1_to_t_uint256(expr_920)
            let expr_921 := fun_mul_2038(expr_919_self, _249)
            /// @src 1:9042,9082
            let expr_922 := fun_sub_2010(expr_917_self, expr_921)
            /// @src 1:9018,9082
            let var_balance1Adjusted_912 := expr_922
            /// @src 1:9100,9116
            let _250 := var_balance0Adjusted_899
            let expr_925 := _250
            /// @src 1:9100,9120
            let expr_926_self := expr_925
            let expr_926_functionIdentifier := 2038
            /// @src 1:9121,9137
            let _251 := var_balance1Adjusted_912
            let expr_927 := _251
            /// @src 1:9100,9138
            let expr_928 := fun_mul_2038(expr_926_self, expr_927)
            /// @src 1:9147,9156
            let _252 := var__reserve0_752
            let expr_931 := _252
            /// @src 1:9142,9157
            let expr_932 := convert_t_uint112_to_t_uint256(expr_931)
            /// @src 1:9142,9161
            let expr_933_self := expr_932
            let expr_933_functionIdentifier := 2038
            /// @src 1:9162,9171
            let _253 := var__reserve1_754
            let expr_934 := _253
            /// @src 1:9142,9172
            let _254 := convert_t_uint112_to_t_uint256(expr_934)
            let expr_935 := fun_mul_2038(expr_933_self, _254)
            /// @src 1:9142,9176
            let expr_936_self := expr_935
            let expr_936_functionIdentifier := 2038
            /// @src 1:9177,9184
            let expr_939 := 0x0f4240
            /// @src 1:9142,9185
            let _255 := convert_t_rational_1000000_by_1_to_t_uint256(expr_939)
            let expr_940 := fun_mul_2038(expr_936_self, _255)
            /// @src 1:9100,9185
            let expr_941 := iszero(lt(cleanup_t_uint256(expr_928), cleanup_t_uint256(expr_940)))
            /// @src 1:9092,9202
            require_helper_t_stringliteral_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af(expr_941)
            /// @src 1:9223,9230
            let expr_946_functionIdentifier := 296
            /// @src 1:9231,9239
            let _256 := var_balance0_770
            let expr_947 := _256
            /// @src 1:9241,9249
            let _257 := var_balance1_773
            let expr_948 := _257
            /// @src 1:9251,9260
            let _258 := var__reserve0_752
            let expr_949 := _258
            /// @src 1:9262,9271
            let _259 := var__reserve1_754
            let expr_950 := _259
            fun__update_296(expr_947, expr_948, expr_949, expr_950)
            /// @src 1:9292,9302
            let expr_955 := caller()
            /// @src 1:9304,9313
            let _260 := var_amount0In_856
            let expr_956 := _260
            /// @src 1:9315,9324
            let _261 := var_amount1In_872
            let expr_957 := _261
            /// @src 1:9326,9336
            let _262 := var_amount0Out_728
            let expr_958 := _262
            /// @src 1:9338,9348
            let _263 := var_amount1Out_730
            let expr_959 := _263
            /// @src 1:9350,9352
            let _264 := var_to_732
            let expr_960 := _264
            /// @src 1:9287,9353
            let _265 := 0xd78ad95fa46c994b6551d0da85fc275fe613ce37657fb8d5e3d130840159d822
            {
                let _266 := allocate_unbounded()
                let _267 := abi_encode_tuple_t_uint256_t_uint256_t_uint256_t_uint256__to_t_uint256_t_uint256_t_uint256_t_uint256__fromStack(_266 , expr_956, expr_957, expr_958, expr_959)
                log3(_266, sub(_267, _266) , _265, expr_955, expr_960)
            }
        }

        function fun_sync_1045() {
            /// @src 1:9790,9955

            modifier_lock_1019()
        }

        function fun_sync_1045_inner() {
            /// @src 1:9790,9955

            /// @src 1:9839,9846
            let expr_1021_functionIdentifier := 296
            /// @src 1:9854,9860
            let _292 := read_from_storage_split_offset_0_t_address(0x06)
            let expr_1023 := _292
            /// @src 1:9847,9861
            let expr_1024_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_1023)
            /// @src 1:9847,9871
            let expr_1025_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_1024_address)
            let expr_1025_functionSelector := 0x70a08231
            /// @src 1:9880,9884
            let expr_1028_address := address()
            /// @src 1:9872,9885
            let expr_1029 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_1028_address)
            /// @src 1:9847,9886
            if iszero(extcodesize(expr_1025_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _293 := allocate_unbounded()
            mstore(_293, shift_left_224(expr_1025_functionSelector))
            let _294 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_293, 4) , expr_1029)

            let _295 := staticcall(gas(), expr_1025_address,  _293, sub(_294, _293), _293, 32)

            if iszero(_295) { revert_forward_1() }

            let expr_1030
            if _295 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_293, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_1030 :=  abi_decode_tuple_t_uint256_fromMemory(_293, add(_293, returndatasize()))
            }
            /// @src 1:9895,9901
            let _296 := read_from_storage_split_offset_0_t_address(0x07)
            let expr_1032 := _296
            /// @src 1:9888,9902
            let expr_1033_address := convert_t_address_to_t_contract$_IERC20_$1522(expr_1032)
            /// @src 1:9888,9912
            let expr_1034_address := convert_t_contract$_IERC20_$1522_to_t_address(expr_1033_address)
            let expr_1034_functionSelector := 0x70a08231
            /// @src 1:9921,9925
            let expr_1037_address := address()
            /// @src 1:9913,9926
            let expr_1038 := convert_t_contract$_UniswapV2Pair_$1046_to_t_address(expr_1037_address)
            /// @src 1:9888,9927
            if iszero(extcodesize(expr_1034_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _297 := allocate_unbounded()
            mstore(_297, shift_left_224(expr_1034_functionSelector))
            let _298 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_297, 4) , expr_1038)

            let _299 := staticcall(gas(), expr_1034_address,  _297, sub(_298, _297), _297, 32)

            if iszero(_299) { revert_forward_1() }

            let expr_1039
            if _299 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_297, returndatasize())

                // decode return parameters from external try-call into retVars
                expr_1039 :=  abi_decode_tuple_t_uint256_fromMemory(_297, add(_297, returndatasize()))
            }
            /// @src 1:9929,9937
            let _300 := read_from_storage_split_offset_0_t_uint112(0x08)
            let expr_1040 := _300
            /// @src 1:9939,9947
            let _301 := read_from_storage_split_offset_14_t_uint112(0x08)
            let expr_1041 := _301
            fun__update_296(expr_1030, expr_1039, expr_1040, expr_1041)

        }

        function fun_transferFrom_1357(var_from_1304, var_to_1306, var_value_1308) -> var__1312 {
            /// @src 0:2574,2883
            /// @src 0:2661,2665
            let zero_t_bool_373 := zero_value_for_split_t_bool()
            var__1312 := zero_t_bool_373

            /// @src 0:2681,2690
            let _374 := 0x02
            let expr_1314 := _374
            /// @src 0:2691,2695
            let _375 := var_from_1304
            let expr_1315 := _375
            /// @src 0:2681,2696
            let _376 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_1314,expr_1315)
            let _377 := _376
            let expr_1316 := _377
            /// @src 0:2697,2707
            let expr_1318 := caller()
            /// @src 0:2681,2708
            let _378 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1316,expr_1318)
            let _379 := read_from_storage_split_offset_0_t_uint256(_378)
            let expr_1319 := _379
            /// @src 0:2722,2723
            let expr_1324 := 0x01
            /// @src 0:2721,2723
            let expr_1325 := 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
            /// @src 0:2717,2724
            let expr_1326 := convert_t_rational_minus_1_by_1_to_t_int256(expr_1325)
            /// @src 0:2712,2725
            let expr_1327 := convert_t_int256_to_t_uint256(expr_1326)
            /// @src 0:2681,2725
            let expr_1328 := iszero(eq(cleanup_t_uint256(expr_1319), cleanup_t_uint256(expr_1327)))
            /// @src 0:2677,2820
            if expr_1328 {
                /// @src 0:2771,2780
                let _380 := 0x02
                let expr_1335 := _380
                /// @src 0:2781,2785
                let _381 := var_from_1304
                let expr_1336 := _381
                /// @src 0:2771,2786
                let _382 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_1335,expr_1336)
                let _383 := _382
                let expr_1337 := _383
                /// @src 0:2787,2797
                let expr_1339 := caller()
                /// @src 0:2771,2798
                let _384 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1337,expr_1339)
                let _385 := read_from_storage_split_offset_0_t_uint256(_384)
                let expr_1340 := _385
                /// @src 0:2771,2802
                let expr_1341_self := expr_1340
                let expr_1341_functionIdentifier := 2010
                /// @src 0:2803,2808
                let _386 := var_value_1308
                let expr_1342 := _386
                /// @src 0:2771,2809
                let expr_1343 := fun_sub_2010(expr_1341_self, expr_1342)
                /// @src 0:2741,2750
                let _387 := 0x02
                let expr_1329 := _387
                /// @src 0:2751,2755
                let _388 := var_from_1304
                let expr_1330 := _388
                /// @src 0:2741,2756
                let _389 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_1329,expr_1330)
                let _390 := _389
                let expr_1333 := _390
                /// @src 0:2757,2767
                let expr_1332 := caller()
                /// @src 0:2741,2768
                let _391 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1333,expr_1332)
                /// @src 0:2741,2809
                update_storage_value_offset_0t_uint256_to_t_uint256(_391, expr_1343)
                let expr_1344 := expr_1343
                /// @src 0:2677,2820
            }
            /// @src 0:2829,2838
            let expr_1348_functionIdentifier := 1262
            /// @src 0:2839,2843
            let _392 := var_from_1304
            let expr_1349 := _392
            /// @src 0:2845,2847
            let _393 := var_to_1306
            let expr_1350 := _393
            /// @src 0:2849,2854
            let _394 := var_value_1308
            let expr_1351 := _394
            fun__transfer_1262(expr_1349, expr_1350, expr_1351)
            /// @src 0:2872,2876
            let expr_1354 := 0x01
            /// @src 0:2865,2876
            var__1312 := expr_1354
            leave

        }

        function fun_transfer_1302(var_to_1284, var_value_1286) -> var__1290 {
            /// @src 0:2423,2568
            /// @src 0:2492,2496
            let zero_t_bool_348 := zero_value_for_split_t_bool()
            var__1290 := zero_t_bool_348

            /// @src 0:2508,2517
            let expr_1292_functionIdentifier := 1262
            /// @src 0:2518,2528
            let expr_1294 := caller()
            /// @src 0:2530,2532
            let _349 := var_to_1284
            let expr_1295 := _349
            /// @src 0:2534,2539
            let _350 := var_value_1286
            let expr_1296 := _350
            fun__transfer_1262(expr_1294, expr_1295, expr_1296)
            /// @src 0:2557,2561
            let expr_1299 := 0x01
            /// @src 0:2550,2561
            var__1290 := expr_1299
            leave

        }

        function fun_uqdiv_1845(var_x_1828, var_y_1830) -> var_z_1833 {
            /// @src 9:468,574
            /// @src 9:528,537
            let zero_t_uint224_443 := zero_value_for_split_t_uint224()
            var_z_1833 := zero_t_uint224_443

            /// @src 9:553,554
            let _444 := var_x_1828
            let expr_1836 := _444
            /// @src 9:565,566
            let _445 := var_y_1830
            let expr_1839 := _445
            /// @src 9:557,567
            let expr_1840 := convert_t_uint112_to_t_uint224(expr_1839)
            /// @src 9:553,567
            let expr_1841 := checked_div_t_uint224(expr_1836, expr_1840)

            /// @src 9:549,567
            var_z_1833 := expr_1841
            let expr_1842 := expr_1841

        }

        function getter_fun_DOMAIN_SEPARATOR_1085() -> ret {
            /// @src 0:516,556

            let slot := 3
            let offset := 0

            ret := read_from_storage_split_dynamic_t_bytes32(slot, offset)

        }

        function getter_fun_MINIMUM_LIQUIDITY_24() -> ret_0 {
            /// @src 1:416,471
            ret_0 := constant_MINIMUM_LIQUIDITY_24()
        }

        function getter_fun_PERMIT_TYPEHASH_1089() -> ret_0 {
            /// @src 0:666,783
            ret_0 := constant_PERMIT_TYPEHASH_1089()
        }

        function getter_fun_allowance_1082(key_0, key_1) -> ret {
            /// @src 0:439,509

            let slot := 2
            let offset := 0

            slot := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(slot, key_0)

            slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_1)

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_balanceOf_1075(key_0) -> ret {
            /// @src 0:383,433

            let slot := 1
            let offset := 0

            slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_0)

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_decimals_1067() -> ret_0 {
            /// @src 0:295,339
            ret_0 := constant_decimals_1067()
        }

        function getter_fun_factory_38() -> ret {
            /// @src 1:572,603

            let slot := 5
            let offset := 0

            ret := read_from_storage_split_dynamic_t_address(slot, offset)

        }

        function getter_fun_kLast_59() -> ret {
            /// @src 1:1065,1091

            let slot := 11
            let offset := 0

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_name_1059() -> ret_0 {
            /// @src 0:183,234
            ret_0 := constant_name_1059()
        }

        function getter_fun_nonces_1094(key_0) -> ret {
            /// @src 0:789,836

            let slot := 4
            let offset := 0

            slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_0)

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_price0CumulativeLast_53() -> ret {
            /// @src 1:971,1012

            let slot := 9
            let offset := 0

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_price1CumulativeLast_56() -> ret {
            /// @src 1:1018,1059

            let slot := 10
            let offset := 0

            ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

        }

        function getter_fun_symbol_1063() -> ret_0 {
            /// @src 0:240,289
            ret_0 := constant_symbol_1063()
        }

        function getter_fun_token0_41() -> ret {
            /// @src 1:609,639

            let slot := 6
            let offset := 0

            ret := read_from_storage_split_dynamic_t_address(slot, offset)

        }

        function getter_fun_token1_44() -> ret {
            /// @src 1:645,675

            let slot := 7
            let offset := 0

            ret := read_from_storage_split_dynamic_t_address(slot, offset)

        }

        function getter_fun_totalSupply_1070() -> ret {
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

        function mod_t_uint256(x, y) -> r {
            x := cleanup_t_uint256(x)
            y := cleanup_t_uint256(y)
            if iszero(y) { panic_error_0x12() }
            r := mod(x, y)
        }

        function modifier_lock_1019() {
            /// @src 1:1209,1340

            /// @src 1:1243,1251
            let _289 := read_from_storage_split_offset_0_t_uint256(0x0c)
            let expr_65 := _289
            /// @src 1:1255,1256
            let expr_66 := 0x01
            /// @src 1:1243,1256
            let expr_67 := eq(cleanup_t_uint256(expr_65), convert_t_rational_1_by_1_to_t_uint256(expr_66))
            /// @src 1:1235,1278
            require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_67)
            /// @src 1:1299,1300
            let expr_72 := 0x00
            /// @src 1:1288,1300
            let _290 := convert_t_rational_0_by_1_to_t_uint256(expr_72)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _290)
            let expr_73 := _290
            /// @src 1:1310,1311
            fun_sync_1045_inner()
            /// @src 1:1332,1333
            let expr_77 := 0x01
            /// @src 1:1321,1333
            let _291 := convert_t_rational_1_by_1_to_t_uint256(expr_77)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _291)
            let expr_78 := _291

        }

        function modifier_lock_410(var_liquidity_412, var_to_406) -> _16 {
            /// @src 1:1209,1340
            _16 := var_liquidity_412

            /// @src 1:1243,1251
            let _17 := read_from_storage_split_offset_0_t_uint256(0x0c)
            let expr_65 := _17
            /// @src 1:1255,1256
            let expr_66 := 0x01
            /// @src 1:1243,1256
            let expr_67 := eq(cleanup_t_uint256(expr_65), convert_t_rational_1_by_1_to_t_uint256(expr_66))
            /// @src 1:1235,1278
            require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_67)
            /// @src 1:1299,1300
            let expr_72 := 0x00
            /// @src 1:1288,1300
            let _18 := convert_t_rational_0_by_1_to_t_uint256(expr_72)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _18)
            let expr_73 := _18
            /// @src 1:1310,1311
            _16 := fun_mint_554_inner(var_liquidity_412, var_to_406)
            /// @src 1:1332,1333
            let expr_77 := 0x01
            /// @src 1:1321,1333
            let _19 := convert_t_rational_1_by_1_to_t_uint256(expr_77)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _19)
            let expr_78 := _19

        }

        function modifier_lock_560(var_amount0_562, var_amount1_564, var_to_556) -> _116, _117 {
            /// @src 1:1209,1340
            _116 := var_amount0_562
            _117 := var_amount1_564

            /// @src 1:1243,1251
            let _118 := read_from_storage_split_offset_0_t_uint256(0x0c)
            let expr_65 := _118
            /// @src 1:1255,1256
            let expr_66 := 0x01
            /// @src 1:1243,1256
            let expr_67 := eq(cleanup_t_uint256(expr_65), convert_t_rational_1_by_1_to_t_uint256(expr_66))
            /// @src 1:1235,1278
            require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_67)
            /// @src 1:1299,1300
            let expr_72 := 0x00
            /// @src 1:1288,1300
            let _119 := convert_t_rational_0_by_1_to_t_uint256(expr_72)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _119)
            let expr_73 := _119
            /// @src 1:1310,1311
            _116, _117 := fun_burn_726_inner(var_amount0_562, var_amount1_564, var_to_556)
            /// @src 1:1332,1333
            let expr_77 := 0x01
            /// @src 1:1321,1333
            let _120 := convert_t_rational_1_by_1_to_t_uint256(expr_77)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _120)
            let expr_78 := _120

        }

        function modifier_lock_738(var_amount0Out_728, var_amount1Out_730, var_to_732, var_data_734_offset, var_data_734_length) {
            /// @src 1:1209,1340

            /// @src 1:1243,1251
            let _187 := read_from_storage_split_offset_0_t_uint256(0x0c)
            let expr_65 := _187
            /// @src 1:1255,1256
            let expr_66 := 0x01
            /// @src 1:1243,1256
            let expr_67 := eq(cleanup_t_uint256(expr_65), convert_t_rational_1_by_1_to_t_uint256(expr_66))
            /// @src 1:1235,1278
            require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_67)
            /// @src 1:1299,1300
            let expr_72 := 0x00
            /// @src 1:1288,1300
            let _188 := convert_t_rational_0_by_1_to_t_uint256(expr_72)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _188)
            let expr_73 := _188
            /// @src 1:1310,1311
            fun_swap_964_inner(var_amount0Out_728, var_amount1Out_730, var_to_732, var_data_734_offset, var_data_734_length)
            /// @src 1:1332,1333
            let expr_77 := 0x01
            /// @src 1:1321,1333
            let _189 := convert_t_rational_1_by_1_to_t_uint256(expr_77)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _189)
            let expr_78 := _189

        }

        function modifier_lock_970(var_to_966) {
            /// @src 1:1209,1340

            /// @src 1:1243,1251
            let _268 := read_from_storage_split_offset_0_t_uint256(0x0c)
            let expr_65 := _268
            /// @src 1:1255,1256
            let expr_66 := 0x01
            /// @src 1:1243,1256
            let expr_67 := eq(cleanup_t_uint256(expr_65), convert_t_rational_1_by_1_to_t_uint256(expr_66))
            /// @src 1:1235,1278
            require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_67)
            /// @src 1:1299,1300
            let expr_72 := 0x00
            /// @src 1:1288,1300
            let _269 := convert_t_rational_0_by_1_to_t_uint256(expr_72)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _269)
            let expr_73 := _269
            /// @src 1:1310,1311
            fun_skim_1015_inner(var_to_966)
            /// @src 1:1332,1333
            let expr_77 := 0x01
            /// @src 1:1321,1333
            let _270 := convert_t_rational_1_by_1_to_t_uint256(expr_77)
            update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _270)
            let expr_78 := _270

        }

        function panic_error_0x11() {
            mstore(0, 35408467139433450592217433187231851964531694900788300625387963629091585785856)
            mstore(4, 0x11)
            revert(0, 0x24)
        }

        function panic_error_0x12() {
            mstore(0, 35408467139433450592217433187231851964531694900788300625387963629091585785856)
            mstore(4, 0x12)
            revert(0, 0x24)
        }

        function panic_error_0x41() {
            mstore(0, 35408467139433450592217433187231851964531694900788300625387963629091585785856)
            mstore(4, 0x41)
            revert(0, 0x24)
        }

        function prepare_store_t_address(value) -> ret {
            ret := value
        }

        function prepare_store_t_uint112(value) -> ret {
            ret := value
        }

        function prepare_store_t_uint256(value) -> ret {
            ret := value
        }

        function prepare_store_t_uint32(value) -> ret {
            ret := value
        }

        function read_from_storage_split_dynamic_t_address(slot, offset) -> value {
            value := extract_from_storage_value_dynamict_address(sload(slot), offset)

        }

        function read_from_storage_split_dynamic_t_bytes32(slot, offset) -> value {
            value := extract_from_storage_value_dynamict_bytes32(sload(slot), offset)

        }

        function read_from_storage_split_dynamic_t_uint256(slot, offset) -> value {
            value := extract_from_storage_value_dynamict_uint256(sload(slot), offset)

        }

        function read_from_storage_split_offset_0_t_address(slot) -> value {
            value := extract_from_storage_value_offset_0t_address(sload(slot))

        }

        function read_from_storage_split_offset_0_t_bytes32(slot) -> value {
            value := extract_from_storage_value_offset_0t_bytes32(sload(slot))

        }

        function read_from_storage_split_offset_0_t_uint112(slot) -> value {
            value := extract_from_storage_value_offset_0t_uint112(sload(slot))

        }

        function read_from_storage_split_offset_0_t_uint256(slot) -> value {
            value := extract_from_storage_value_offset_0t_uint256(sload(slot))

        }

        function read_from_storage_split_offset_14_t_uint112(slot) -> value {
            value := extract_from_storage_value_offset_14t_uint112(sload(slot))

        }

        function read_from_storage_split_offset_28_t_uint32(slot) -> value {
            value := extract_from_storage_value_offset_28t_uint32(sload(slot))

        }

        function require_helper_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
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

        function require_helper_t_stringliteral_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
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

        function require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() {
            revert(0, 0)
        }

        function revert_error_15abf5612cd996bc235ba1e55a4a30ac60e6bb601ff7ba4ad3f179b6be8d0490() {
            revert(0, 0)
        }

        function revert_error_1b9f4a0a5773e33b91aa01db23bf8c55fce1411167c872835e7fa00a4f17d46d() {
            revert(0, 0)
        }

        function revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74() {
            revert(0, 0)
        }

        function revert_error_81385d8c0b31fffe14be1da910c8bd3a80be4cfa248e04f42ec0faea3132a8ef() {
            revert(0, 0)
        }

        function revert_error_c1322bf8034eace5e0b5c7295db60986aa89aae5e0ea0873e4689e076861a5db() {
            revert(0, 0)
        }

        function revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() {
            revert(0, 0)
        }

        function revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() {
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

        function shift_left_112(value) -> newValue {
            newValue :=

            shl(112, value)

        }

        function shift_left_224(value) -> newValue {
            newValue :=

            shl(224, value)

        }

        function shift_right_0_unsigned(value) -> newValue {
            newValue :=

            shr(0, value)

        }

        function shift_right_112_unsigned(value) -> newValue {
            newValue :=

            shr(112, value)

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

        function store_literal_in_memory_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: INSUFFICIENT_OUTPUT_A")

            mstore(add(memPtr, 32), "MOUNT")

        }

        function store_literal_in_memory_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255(memPtr) {

            mstore(add(memPtr, 0), "UNI-V2")

        }

        function store_literal_in_memory_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: INSUFFICIENT_INPUT_AM")

            mstore(add(memPtr, 32), "OUNT")

        }

        function store_literal_in_memory_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd(memPtr) {

            mstore(add(memPtr, 0), "ds-math-mul-overflow")

        }

        function store_literal_in_memory_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: INVALID_TO")

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

        function store_literal_in_memory_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: INSUFFICIENT_LIQUIDIT")

            mstore(add(memPtr, 32), "Y")

        }

        function store_literal_in_memory_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: EXPIRED")

        }

        function store_literal_in_memory_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: LOCKED")

        }

        function store_literal_in_memory_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: K")

        }

        function store_literal_in_memory_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: INSUFFICIENT_LIQUIDIT")

            mstore(add(memPtr, 32), "Y_BURNED")

        }

        function store_literal_in_memory_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: INSUFFICIENT_LIQUIDIT")

            mstore(add(memPtr, 32), "Y_MINTED")

        }

        function store_literal_in_memory_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: FORBIDDEN")

        }

        function store_literal_in_memory_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: OVERFLOW")

        }

        function store_literal_in_memory_a9059cbb2ab09eb219583f4a59a5d0623ade346d962bcd4e46b11da047c9049b(memPtr) {

            mstore(add(memPtr, 0), "transfer(address,uint256)")

        }

        function store_literal_in_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738(memPtr) {

            mstore(add(memPtr, 0), "Uniswap V2")

        }

        function store_literal_in_memory_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: TRANSFER_FAILED")

        }

        function update_byte_slice_14_shift_0(value, toInsert) -> result {
            let mask := 0xffffffffffffffffffffffffffff
            toInsert := shift_left_0(toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_byte_slice_14_shift_14(value, toInsert) -> result {
            let mask := 0xffffffffffffffffffffffffffff0000000000000000000000000000
            toInsert := shift_left_112(toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_byte_slice_20_shift_0(value, toInsert) -> result {
            let mask := 0xffffffffffffffffffffffffffffffffffffffff
            toInsert := shift_left_0(toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_byte_slice_32_shift_0(value, toInsert) -> result {
            let mask := 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
            toInsert := shift_left_0(toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_byte_slice_4_shift_28(value, toInsert) -> result {
            let mask := 0xffffffff00000000000000000000000000000000000000000000000000000000
            toInsert := shift_left_224(toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_storage_value_offset_0t_address_to_t_address(slot, value_0) {
            let convertedValue_0 := convert_t_address_to_t_address(value_0)
            sstore(slot, update_byte_slice_20_shift_0(sload(slot), prepare_store_t_address(convertedValue_0)))
        }

        function update_storage_value_offset_0t_uint112_to_t_uint112(slot, value_0) {
            let convertedValue_0 := convert_t_uint112_to_t_uint112(value_0)
            sstore(slot, update_byte_slice_14_shift_0(sload(slot), prepare_store_t_uint112(convertedValue_0)))
        }

        function update_storage_value_offset_0t_uint256_to_t_uint256(slot, value_0) {
            let convertedValue_0 := convert_t_uint256_to_t_uint256(value_0)
            sstore(slot, update_byte_slice_32_shift_0(sload(slot), prepare_store_t_uint256(convertedValue_0)))
        }

        function update_storage_value_offset_14t_uint112_to_t_uint112(slot, value_0) {
            let convertedValue_0 := convert_t_uint112_to_t_uint112(value_0)
            sstore(slot, update_byte_slice_14_shift_14(sload(slot), prepare_store_t_uint112(convertedValue_0)))
        }

        function update_storage_value_offset_28t_uint32_to_t_uint32(slot, value_0) {
            let convertedValue_0 := convert_t_uint32_to_t_uint32(value_0)
            sstore(slot, update_byte_slice_4_shift_28(sload(slot), prepare_store_t_uint32(convertedValue_0)))
        }

        function validator_revert_t_address(value) {
            if iszero(eq(value, cleanup_t_address(value))) { revert(0, 0) }
        }

        function validator_revert_t_bool(value) {
            if iszero(eq(value, cleanup_t_bool(value))) { revert(0, 0) }
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

        function zero_value_for_split_t_bytes_memory_ptr() -> ret {
            ret := 96
        }

        function zero_value_for_split_t_uint112() -> ret {
            ret := 0
        }

        function zero_value_for_split_t_uint224() -> ret {
            ret := 0
        }

        function zero_value_for_split_t_uint256() -> ret {
            ret := 0
        }

        function zero_value_for_split_t_uint32() -> ret {
            ret := 0
        }

    }

    data ".metadata" hex"a2646970667358221220779a54c21f08ddd80756cb4af3ea621f9f487557e96d55e48bce5741db3e489864736f6c63430008060033"
}

