/*=====================================================*
 *                       WARNING                       *
 *  Solidity to Yul compilation is still EXPERIMENTAL  *
 *       It can result in LOSS OF FUNDS or worse       *
 *                !USE AT YOUR OWN RISK!               *
 *=====================================================*/



object "UniswapV2Factory_195_deployed" {
    code {
        /// @src 1:101,1922
        mstore(64, 128)

        if iszero(lt(calldatasize(), 4))
        {
            let selector := shift_right_224_unsigned(calldataload(0))
            switch selector

            case 0x017e7e58
            {
                // feeTo()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_feeTo_8()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x094b7415
            {
                // feeToSetter()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  getter_fun_feeToSetter_11()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x1e3dd18b
            {
                // allPairs(uint256)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_uint256(4, calldatasize())
                let ret_0 :=  getter_fun_allPairs_22(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0x574f2ba3
            {
                // allPairsLength()

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                abi_decode_tuple_(4, calldatasize())
                let ret_0 :=  fun_allPairsLength_43()
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xa2e74af6
            {
                // setFeeToSetter(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                fun_setFeeToSetter_194(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                return(memPos, sub(memEnd, memPos))
            }

            case 0xc9c65396
            {
                // createPair(address,address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_address(4, calldatasize())
                let ret_0 :=  fun_createPair_156(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xe6a43905
            {
                // getPair(address,address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_addresst_address(4, calldatasize())
                let ret_0 :=  getter_fun_getPair_18(param_0, param_1)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))
            }

            case 0xf46901ed
            {
                // setFeeTo(address)

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                fun_setFeeTo_175(param_0)
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

        function abi_decode_t_uint256(offset, end) -> value {
            value := calldataload(offset)
            validator_revert_t_uint256(value)
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

        function abi_decode_tuple_t_uint256(headStart, dataEnd) -> value0 {
            if slt(sub(dataEnd, headStart), 32) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

            {

                let offset := 0

                value0 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
            }

        }

        function abi_encode_t_address_to_t_address_fromStack(value, pos) {
            mstore(pos, cleanup_t_address(value))
        }

        function abi_encode_t_address_to_t_address_nonPadded_inplace_fromStack(value, pos) {
            mstore(pos, leftAlign_t_address(cleanup_t_address(value)))
        }

        function abi_encode_t_stringliteral_1af2ec9097b2f8bc2dcfea53a9ab4b2cdab42fa29e9a9e04dcb14b4efcc8aa70_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 30)
            store_literal_in_memory_1af2ec9097b2f8bc2dcfea53a9ab4b2cdab42fa29e9a9e04dcb14b4efcc8aa70(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 20)
            store_literal_in_memory_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_7597a3317d1f47998beb266ffa8b5f1f9be064321f01552ef08c1fe9eeb777db_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 22)
            store_literal_in_memory_7597a3317d1f47998beb266ffa8b5f1f9be064321f01552ef08c1fe9eeb777db(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_9fd3496d51391106f97d9c12d75d9ef2543a217eeaf4b9c52c6fdbe23f45a5ae_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 23)
            store_literal_in_memory_9fd3496d51391106f97d9c12d75d9ef2543a217eeaf4b9c52c6fdbe23f45a5ae(pos)
            end := add(pos, 32)
        }

        function abi_encode_t_uint256_to_t_uint256_fromStack(value, pos) {
            mstore(pos, cleanup_t_uint256(value))
        }

        function abi_encode_tuple__to__fromStack(headStart ) -> tail {
            tail := add(headStart, 0)

        }

        function abi_encode_tuple_packed_t_address_t_address__to_t_address_t_address__nonPadded_inplace_fromStack(pos , value0, value1) -> end {

            abi_encode_t_address_to_t_address_nonPadded_inplace_fromStack(value0,  pos)
            pos := add(pos, 20)

            abi_encode_t_address_to_t_address_nonPadded_inplace_fromStack(value1,  pos)
            pos := add(pos, 20)

            end := pos
        }

        function abi_encode_tuple_t_address__to_t_address__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_address_to_t_address_fromStack(value0,  add(headStart, 0))

        }

        function abi_encode_tuple_t_address_t_address__to_t_address_t_address__fromStack(headStart , value0, value1) -> tail {
            tail := add(headStart, 64)

            abi_encode_t_address_to_t_address_fromStack(value0,  add(headStart, 0))

            abi_encode_t_address_to_t_address_fromStack(value1,  add(headStart, 32))

        }

        function abi_encode_tuple_t_address_t_uint256__to_t_address_t_uint256__fromStack(headStart , value0, value1) -> tail {
            tail := add(headStart, 64)

            abi_encode_t_address_to_t_address_fromStack(value0,  add(headStart, 0))

            abi_encode_t_uint256_to_t_uint256_fromStack(value1,  add(headStart, 32))

        }

        function abi_encode_tuple_t_stringliteral_1af2ec9097b2f8bc2dcfea53a9ab4b2cdab42fa29e9a9e04dcb14b4efcc8aa70__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_1af2ec9097b2f8bc2dcfea53a9ab4b2cdab42fa29e9a9e04dcb14b4efcc8aa70_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_7597a3317d1f47998beb266ffa8b5f1f9be064321f01552ef08c1fe9eeb777db__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_7597a3317d1f47998beb266ffa8b5f1f9be064321f01552ef08c1fe9eeb777db_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_9fd3496d51391106f97d9c12d75d9ef2543a217eeaf4b9c52c6fdbe23f45a5ae__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_9fd3496d51391106f97d9c12d75d9ef2543a217eeaf4b9c52c6fdbe23f45a5ae_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_uint256__to_t_uint256__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

        }

        function allocate_memory(size) -> memPtr {
            memPtr := allocate_unbounded()
            finalize_allocation(memPtr, size)
        }

        function allocate_unbounded() -> memPtr {
            memPtr := mload(64)
        }

        function array_dataslot_t_array$_t_address_$dyn_storage(ptr) -> data {
            data := ptr

            mstore(0, ptr)
            data := keccak256(0, 0x20)

        }

        function array_dataslot_t_array$_t_address_$dyn_storage_ptr(ptr) -> data {
            data := ptr

            mstore(0, ptr)
            data := keccak256(0, 0x20)

        }

        function array_dataslot_t_bytes_memory_ptr(ptr) -> data {
            data := ptr

            data := add(ptr, 0x20)

        }

        function array_dataslot_t_bytes_storage_ptr(ptr) -> data {
            data := ptr

            mstore(0, ptr)
            data := keccak256(0, 0x20)

        }

        function array_length_t_array$_t_address_$dyn_storage(value) -> length {

            length := sload(value)

        }

        function array_length_t_array$_t_address_$dyn_storage_ptr(value) -> length {

            length := sload(value)

        }

        function array_length_t_bytes_memory_ptr(value) -> length {

            length := mload(value)

        }

        function array_push_from_t_address_to_t_array$_t_address_$dyn_storage_ptr(array , value0) {

            let oldLen := sload(array)
            if iszero(lt(oldLen, 18446744073709551616)) { panic_error_0x41() }
            sstore(array, add(oldLen, 1))
            let slot, offset := storage_array_index_access_t_array$_t_address_$dyn_storage_ptr(array, oldLen)
            update_storage_value_t_address_to_t_address(slot, offset , value0)

        }
        function array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, length) -> updated_pos {
            mstore(pos, length)
            updated_pos := add(pos, 0x20)
        }

        function cleanup_from_storage_t_address(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffff)
        }

        function cleanup_t_address(value) -> cleaned {
            cleaned := cleanup_t_uint160(value)
        }

        function cleanup_t_uint160(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffff)
        }

        function cleanup_t_uint256(value) -> cleaned {
            cleaned := value
        }

        function convert_array_t_array$_t_address_$dyn_storage_to_t_array$_t_address_$dyn_storage_ptr(value) -> converted  {
            converted := value

        }

        function convert_t_address_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_address(value)
        }

        function convert_t_address_to_t_contract$_IUniswapV2Pair_$1922(value) -> converted {
            converted := convert_t_uint160_to_t_contract$_IUniswapV2Pair_$1922(value)
        }

        function convert_t_contract$_IUniswapV2Pair_$1922_to_t_address(value) -> converted {
            converted := convert_t_contract$_IUniswapV2Pair_$1922_to_t_uint160(value)
        }

        function convert_t_contract$_IUniswapV2Pair_$1922_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_rational_0_by_1_to_t_address(value) -> converted {
            converted := convert_t_rational_0_by_1_to_t_uint160(value)
        }

        function convert_t_rational_0_by_1_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_uint160_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_uint160(value)
        }

        function convert_t_uint160_to_t_contract$_IUniswapV2Pair_$1922(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function convert_t_uint160_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function extract_from_storage_value_dynamict_address(slot_value, offset) -> value {
            value := cleanup_from_storage_t_address(shift_right_unsigned_dynamic(mul(offset, 8), slot_value))
        }

        function extract_from_storage_value_offset_0t_address(slot_value) -> value {
            value := cleanup_from_storage_t_address(shift_right_0_unsigned(slot_value))
        }

        function finalize_allocation(memPtr, size) {
            let newFreePtr := add(memPtr, round_up_to_mul_of_32(size))
            // protect against overflow
            if or(gt(newFreePtr, 0xffffffffffffffff), lt(newFreePtr, memPtr)) { panic_error_0x41() }
            mstore(64, newFreePtr)
        }

        function fun_allPairsLength_43() -> var__37 {
            /// @src 1:516,619
            /// @src 1:574,578
            let zero_t_uint256_1 := zero_value_for_split_t_uint256()
            var__37 := zero_t_uint256_1

            /// @src 1:597,605
            let _2_slot := 0x03
            let expr_39_slot := _2_slot
            /// @src 1:597,612
            let expr_40 := array_length_t_array$_t_address_$dyn_storage(expr_39_slot)
            /// @src 1:590,612
            var__37 := expr_40
            leave

        }

        function fun_createPair_156(var_tokenA_45, var_tokenB_47) -> var_pair_51 {
            /// @src 1:625,1582
            /// @src 1:704,716
            let zero_t_address_3 := zero_value_for_split_t_address()
            var_pair_51 := zero_t_address_3

            /// @src 1:736,742
            let _4 := var_tokenA_45
            let expr_54 := _4
            /// @src 1:746,752
            let _5 := var_tokenB_47
            let expr_55 := _5
            /// @src 1:736,752
            let expr_56 := iszero(eq(cleanup_t_address(expr_54), cleanup_t_address(expr_55)))
            /// @src 1:728,787
            require_helper_t_stringliteral_1af2ec9097b2f8bc2dcfea53a9ab4b2cdab42fa29e9a9e04dcb14b4efcc8aa70(expr_56)
            /// @src 1:832,838
            let _6 := var_tokenA_45
            let expr_64 := _6
            /// @src 1:841,847
            let _7 := var_tokenB_47
            let expr_65 := _7
            /// @src 1:832,847
            let expr_66 := lt(cleanup_t_address(expr_64), cleanup_t_address(expr_65))
            /// @src 1:832,885
            let expr_73_component_1, expr_73_component_2
            switch expr_66
            case 0 {
                /// @src 1:870,876
                let _8 := var_tokenB_47
                let expr_70 := _8
                /// @src 1:869,885
                let expr_72_component_1 := expr_70
                /// @src 1:878,884
                let _9 := var_tokenA_45
                let expr_71 := _9
                /// @src 1:869,885
                let expr_72_component_2 := expr_71
                /// @src 1:832,885
                expr_73_component_1 := expr_72_component_1
                expr_73_component_2 := expr_72_component_2
            }
            default {
                /// @src 1:851,857
                let _10 := var_tokenA_45
                let expr_67 := _10
                /// @src 1:850,866
                let expr_69_component_1 := expr_67
                /// @src 1:859,865
                let _11 := var_tokenB_47
                let expr_68 := _11
                /// @src 1:850,866
                let expr_69_component_2 := expr_68
                /// @src 1:832,885
                expr_73_component_1 := expr_69_component_1
                expr_73_component_2 := expr_69_component_2
            }
            /// @src 1:797,885
            let var_token0_61 := expr_73_component_1
            let var_token1_63 := expr_73_component_2
            /// @src 1:903,909
            let _12 := var_token0_61
            let expr_76 := _12
            /// @src 1:921,922
            let expr_79 := 0x00
            /// @src 1:913,923
            let expr_80 := convert_t_rational_0_by_1_to_t_address(expr_79)
            /// @src 1:903,923
            let expr_81 := iszero(eq(cleanup_t_address(expr_76), cleanup_t_address(expr_80)))
            /// @src 1:895,951
            require_helper_t_stringliteral_9fd3496d51391106f97d9c12d75d9ef2543a217eeaf4b9c52c6fdbe23f45a5ae(expr_81)
            /// @src 1:969,976
            let _13 := 0x02
            let expr_86 := _13
            /// @src 1:977,983
            let _14 := var_token0_61
            let expr_87 := _14
            /// @src 1:969,984
            let _15 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_address_$_$_of_t_address(expr_86,expr_87)
            let _16 := _15
            let expr_88 := _16
            /// @src 1:985,991
            let _17 := var_token1_63
            let expr_89 := _17
            /// @src 1:969,992
            let _18 := mapping_index_access_t_mapping$_t_address_$_t_address_$_of_t_address(expr_88,expr_89)
            let _19 := read_from_storage_split_offset_0_t_address(_18)
            let expr_90 := _19
            /// @src 1:1004,1005
            let expr_93 := 0x00
            /// @src 1:996,1006
            let expr_94 := convert_t_rational_0_by_1_to_t_address(expr_93)
            /// @src 1:969,1006
            let expr_95 := eq(cleanup_t_address(expr_90), cleanup_t_address(expr_94))
            /// @src 1:961,1033
            require_helper_t_stringliteral_7597a3317d1f47998beb266ffa8b5f1f9be064321f01552ef08c1fe9eeb777db(expr_95)
            /// @src 1:1097,1129

            let _20 := datasize("UniswapV2Pair_1242")
            let expr_104_mpos := allocate_memory(add(_20, 32))
            mstore(expr_104_mpos, _20)
            datacopy(add(expr_104_mpos, 32), dataoffset("UniswapV2Pair_1242"), _20)
            /// @src 1:1073,1129
            let var_bytecode_100_mpos := expr_104_mpos
            /// @src 1:1181,1187
            let _21 := var_token0_61
            let expr_111 := _21
            /// @src 1:1189,1195
            let _22 := var_token1_63
            let expr_112 := _22
            /// @src 1:1164,1196

            let expr_113_mpos := allocate_unbounded()
            let _23 := add(expr_113_mpos, 0x20)

            let _24 := abi_encode_tuple_packed_t_address_t_address__to_t_address_t_address__nonPadded_inplace_fromStack(_23, expr_111, expr_112)
            mstore(expr_113_mpos, sub(_24, add(expr_113_mpos, 0x20)))
            finalize_allocation(expr_113_mpos, sub(_24, expr_113_mpos))
            /// @src 1:1154,1197

             let expr_114 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_113_mpos), array_length_t_bytes_memory_ptr(expr_113_mpos))
            /// @src 1:1139,1197
            invalid()
            let var_salt_107 := expr_114
            /// @src 1:1207,1300
            {
                var_pair_51 := create2(0, add(var_bytecode_100_mpos, 32), mload(var_bytecode_100_mpos), var_salt_107)
            }

            /// @src 1:1324,1328
            let _25 := var_pair_51
            let expr_118 := _25
            /// @src 1:1309,1329
            let expr_119_address := convert_t_address_to_t_contract$_IUniswapV2Pair_$1922(expr_118)
            /// @src 1:1309,1340
            let expr_120_address := convert_t_contract$_IUniswapV2Pair_$1922_to_t_address(expr_119_address)
            let expr_120_functionSelector := 0x485cc955
            /// @src 1:1341,1347
            let _26 := var_token0_61
            let expr_121 := _26
            /// @src 1:1349,1355
            let _27 := var_token1_63
            let expr_122 := _27
            /// @src 1:1309,1356
            if iszero(extcodesize(expr_120_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

            // storage for arguments and returned data
            let _28 := allocate_unbounded()
            mstore(_28, shift_left_224(expr_120_functionSelector))
            let _29 := abi_encode_tuple_t_address_t_address__to_t_address_t_address__fromStack(add(_28, 4) , expr_121, expr_122)

            let _30 := call(gas(), expr_120_address,  0,  _28, sub(_29, _28), _28, 0)

            if iszero(_30) { revert_forward_1() }

            if _30 {

                // update freeMemoryPointer according to dynamic return size
                finalize_allocation(_28, returndatasize())

                // decode return parameters from external try-call into retVars
                abi_decode_tuple__fromMemory(_28, add(_28, returndatasize()))
            }
            /// @src 1:1392,1396
            let _31 := var_pair_51
            let expr_130 := _31
            /// @src 1:1366,1373
            let _32 := 0x02
            let expr_125 := _32
            /// @src 1:1374,1380
            let _33 := var_token0_61
            let expr_126 := _33
            /// @src 1:1366,1381
            let _34 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_address_$_$_of_t_address(expr_125,expr_126)
            let _35 := _34
            let expr_128 := _35
            /// @src 1:1382,1388
            let _36 := var_token1_63
            let expr_127 := _36
            /// @src 1:1366,1389
            let _37 := mapping_index_access_t_mapping$_t_address_$_t_address_$_of_t_address(expr_128,expr_127)
            /// @src 1:1366,1396
            update_storage_value_offset_0t_address_to_t_address(_37, expr_130)
            let expr_131 := expr_130
            /// @src 1:1432,1436
            let _38 := var_pair_51
            let expr_138 := _38
            /// @src 1:1406,1413
            let _39 := 0x02
            let expr_133 := _39
            /// @src 1:1414,1420
            let _40 := var_token1_63
            let expr_134 := _40
            /// @src 1:1406,1421
            let _41 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_address_$_$_of_t_address(expr_133,expr_134)
            let _42 := _41
            let expr_136 := _42
            /// @src 1:1422,1428
            let _43 := var_token0_61
            let expr_135 := _43
            /// @src 1:1406,1429
            let _44 := mapping_index_access_t_mapping$_t_address_$_t_address_$_of_t_address(expr_136,expr_135)
            /// @src 1:1406,1436
            update_storage_value_offset_0t_address_to_t_address(_44, expr_138)
            let expr_139 := expr_138
            /// @src 1:1491,1499
            let _45_slot := 0x03
            let expr_141_slot := _45_slot
            /// @src 1:1491,1504
            let expr_143_self_slot := convert_array_t_array$_t_address_$dyn_storage_to_t_array$_t_address_$dyn_storage_ptr(expr_141_slot)
            /// @src 1:1505,1509
            let _46 := var_pair_51
            let expr_144 := _46
            /// @src 1:1491,1510
            array_push_from_t_address_to_t_array$_t_address_$dyn_storage_ptr(expr_143_self_slot, expr_144)
            /// @src 1:1537,1543
            let _47 := var_token0_61
            let expr_148 := _47
            /// @src 1:1545,1551
            let _48 := var_token1_63
            let expr_149 := _48
            /// @src 1:1553,1557
            let _49 := var_pair_51
            let expr_150 := _49
            /// @src 1:1559,1567
            let _50_slot := 0x03
            let expr_151_slot := _50_slot
            /// @src 1:1559,1574
            let expr_152 := array_length_t_array$_t_address_$dyn_storage(expr_151_slot)
            /// @src 1:1525,1575
            let _51 := 0x0d3648bd0f6ba80134a33ba9275ac585d9d315f0ad8355cddefde31afa28d0e9
            {
                let _52 := allocate_unbounded()
                let _53 := abi_encode_tuple_t_address_t_uint256__to_t_address_t_uint256__fromStack(_52 , expr_150, expr_152)
                log3(_52, sub(_53, _52) , _51, expr_148, expr_149)
            }
        }

        function fun_setFeeToSetter_194(var__feeToSetter_177) {
            /// @src 1:1745,1920

            /// @src 1:1827,1837
            let expr_183 := caller()
            /// @src 1:1841,1852
            let _56 := read_from_storage_split_offset_0_t_address(0x01)
            let expr_184 := _56
            /// @src 1:1827,1852
            let expr_185 := eq(cleanup_t_address(expr_183), cleanup_t_address(expr_184))
            /// @src 1:1819,1877
            require_helper_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(expr_185)
            /// @src 1:1901,1913
            let _57 := var__feeToSetter_177
            let expr_190 := _57
            /// @src 1:1887,1913
            update_storage_value_offset_0t_address_to_t_address(0x01, expr_190)
            let expr_191 := expr_190

        }

        function fun_setFeeTo_175(var__feeTo_158) {
            /// @src 1:1588,1739

            /// @src 1:1658,1668
            let expr_164 := caller()
            /// @src 1:1672,1683
            let _54 := read_from_storage_split_offset_0_t_address(0x01)
            let expr_165 := _54
            /// @src 1:1658,1683
            let expr_166 := eq(cleanup_t_address(expr_164), cleanup_t_address(expr_165))
            /// @src 1:1650,1708
            require_helper_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(expr_166)
            /// @src 1:1726,1732
            let _55 := var__feeTo_158
            let expr_171 := _55
            /// @src 1:1718,1732
            update_storage_value_offset_0t_address_to_t_address(0x00, expr_171)
            let expr_172 := expr_171

        }

        function getter_fun_allPairs_22(key_0) -> ret {
            /// @src 1:308,342

            let slot := 3
            let offset := 0

            if iszero(lt(key_0, array_length_t_array$_t_address_$dyn_storage(slot))) { revert(0, 0) }

            slot, offset := storage_array_index_access_t_array$_t_address_$dyn_storage(slot, key_0)

            ret := read_from_storage_split_dynamic_t_address(slot, offset)

        }

        function getter_fun_feeToSetter_11() -> ret {
            /// @src 1:189,224

            let slot := 1
            let offset := 0

            ret := read_from_storage_split_dynamic_t_address(slot, offset)

        }

        function getter_fun_feeTo_8() -> ret {
            /// @src 1:154,183

            let slot := 0
            let offset := 0

            ret := read_from_storage_split_dynamic_t_address(slot, offset)

        }

        function getter_fun_getPair_18(key_0, key_1) -> ret {
            /// @src 1:231,302

            let slot := 2
            let offset := 0

            slot := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_address_$_$_of_t_address(slot, key_0)

            slot := mapping_index_access_t_mapping$_t_address_$_t_address_$_of_t_address(slot, key_1)

            ret := read_from_storage_split_dynamic_t_address(slot, offset)

        }

        function leftAlign_t_address(value) -> aligned {
            aligned := leftAlign_t_uint160(value)
        }

        function leftAlign_t_uint160(value) -> aligned {
            aligned := shift_left_96(value)
        }

        function long_byte_array_index_access_no_checks(array, index) -> slot, offset {

            offset := sub(31, mod(index, 0x20))
            let dataArea := array_dataslot_t_bytes_storage_ptr(array)
            slot := add(dataArea, div(index, 0x20))

        }

        function mapping_index_access_t_mapping$_t_address_$_t_address_$_of_t_address(slot , key) -> dataSlot {
            mstore(0, convert_t_address_to_t_address(key))
            mstore(0x20, slot)
            dataSlot := keccak256(0, 0x40)
        }

        function mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_address_$_$_of_t_address(slot , key) -> dataSlot {
            mstore(0, convert_t_address_to_t_address(key))
            mstore(0x20, slot)
            dataSlot := keccak256(0, 0x40)
        }

        function panic_error_0x32() {
            mstore(0, 35408467139433450592217433187231851964531694900788300625387963629091585785856)
            mstore(4, 0x32)
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

        function read_from_storage_split_dynamic_t_address(slot, offset) -> value {
            value := extract_from_storage_value_dynamict_address(sload(slot), offset)

        }

        function read_from_storage_split_offset_0_t_address(slot) -> value {
            value := extract_from_storage_value_offset_0t_address(sload(slot))

        }

        function require_helper_t_stringliteral_1af2ec9097b2f8bc2dcfea53a9ab4b2cdab42fa29e9a9e04dcb14b4efcc8aa70(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_1af2ec9097b2f8bc2dcfea53a9ab4b2cdab42fa29e9a9e04dcb14b4efcc8aa70__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
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

        function require_helper_t_stringliteral_7597a3317d1f47998beb266ffa8b5f1f9be064321f01552ef08c1fe9eeb777db(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_7597a3317d1f47998beb266ffa8b5f1f9be064321f01552ef08c1fe9eeb777db__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function require_helper_t_stringliteral_9fd3496d51391106f97d9c12d75d9ef2543a217eeaf4b9c52c6fdbe23f45a5ae(condition ) {
            if iszero(condition) {
                let memPtr := allocate_unbounded()
                mstore(memPtr, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_9fd3496d51391106f97d9c12d75d9ef2543a217eeaf4b9c52c6fdbe23f45a5ae__to_t_string_memory_ptr__fromStack(add(memPtr, 4) )
                revert(memPtr, sub(end, memPtr))
            }
        }

        function revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() {
            revert(0, 0)
        }

        function revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74() {
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

        function shift_left_224(value) -> newValue {
            newValue :=

            shl(224, value)

        }

        function shift_left_96(value) -> newValue {
            newValue :=

            shl(96, value)

        }

        function shift_left_dynamic(bits, value) -> newValue {
            newValue :=

            shl(bits, value)

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

        function storage_array_index_access_t_array$_t_address_$dyn_storage(array, index) -> slot, offset {
            let arrayLength := array_length_t_array$_t_address_$dyn_storage(array)
            if iszero(lt(index, arrayLength)) { panic_error_0x32() }

            let dataArea := array_dataslot_t_array$_t_address_$dyn_storage(array)
            slot := add(dataArea, mul(index, 1))
            offset := 0

        }

        function storage_array_index_access_t_array$_t_address_$dyn_storage_ptr(array, index) -> slot, offset {
            let arrayLength := array_length_t_array$_t_address_$dyn_storage_ptr(array)
            if iszero(lt(index, arrayLength)) { panic_error_0x32() }

            let dataArea := array_dataslot_t_array$_t_address_$dyn_storage_ptr(array)
            slot := add(dataArea, mul(index, 1))
            offset := 0

        }

        function store_literal_in_memory_1af2ec9097b2f8bc2dcfea53a9ab4b2cdab42fa29e9a9e04dcb14b4efcc8aa70(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: IDENTICAL_ADDRESSES")

        }

        function store_literal_in_memory_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: FORBIDDEN")

        }

        function store_literal_in_memory_7597a3317d1f47998beb266ffa8b5f1f9be064321f01552ef08c1fe9eeb777db(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: PAIR_EXISTS")

        }

        function store_literal_in_memory_9fd3496d51391106f97d9c12d75d9ef2543a217eeaf4b9c52c6fdbe23f45a5ae(memPtr) {

            mstore(add(memPtr, 0), "UniswapV2: ZERO_ADDRESS")

        }

        function update_byte_slice_20_shift_0(value, toInsert) -> result {
            let mask := 0xffffffffffffffffffffffffffffffffffffffff
            toInsert := shift_left_0(toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_byte_slice_dynamic20(value, shiftBytes, toInsert) -> result {
            let shiftBits := mul(shiftBytes, 8)
            let mask := shift_left_dynamic(shiftBits, 0xffffffffffffffffffffffffffffffffffffffff)
            toInsert := shift_left_dynamic(shiftBits, toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_storage_value_offset_0t_address_to_t_address(slot, value_0) {
            let convertedValue_0 := convert_t_address_to_t_address(value_0)
            sstore(slot, update_byte_slice_20_shift_0(sload(slot), prepare_store_t_address(convertedValue_0)))
        }

        function update_storage_value_t_address_to_t_address(slot, offset, value_0) {
            let convertedValue_0 := convert_t_address_to_t_address(value_0)
            sstore(slot, update_byte_slice_dynamic20(sload(slot), offset, prepare_store_t_address(convertedValue_0)))
        }

        function validator_revert_t_address(value) {
            if iszero(eq(value, cleanup_t_address(value))) { revert(0, 0) }
        }

        function validator_revert_t_uint256(value) {
            if iszero(eq(value, cleanup_t_uint256(value))) { revert(0, 0) }
        }

        function zero_value_for_split_t_address() -> ret {
            ret := 0
        }

        function zero_value_for_split_t_uint256() -> ret {
            ret := 0
        }

    }
    /*=====================================================*
    *                       WARNING                       *
    *  Solidity to Yul compilation is still EXPERIMENTAL  *
    *       It can result in LOSS OF FUNDS or worse       *
    *                !USE AT YOUR OWN RISK!               *
    *=====================================================*/

    object "UniswapV2Pair_1242" {
        code {
            /// @src 2:289,9957
            mstore(64, 128)
            if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }

            constructor_UniswapV2Pair_1242()

            let _1 := allocate_unbounded()
            codecopy(_1, dataoffset("UniswapV2Pair_1242_deployed"), datasize("UniswapV2Pair_1242_deployed"))

            return(_1, datasize("UniswapV2Pair_1242_deployed"))

            function abi_encode_t_address_to_t_address_fromStack(value, pos) {
                mstore(pos, cleanup_t_address(value))
            }

            function abi_encode_t_bytes32_to_t_bytes32_fromStack(value, pos) {
                mstore(pos, cleanup_t_bytes32(value))
            }

            function abi_encode_t_uint256_to_t_uint256_fromStack(value, pos) {
                mstore(pos, cleanup_t_uint256(value))
            }

            function abi_encode_tuple_t_bytes32_t_bytes32_t_bytes32_t_uint256_t_address__to_t_bytes32_t_bytes32_t_bytes32_t_uint256_t_address__fromStack(headStart , value0, value1, value2, value3, value4) -> tail {
                tail := add(headStart, 160)

                abi_encode_t_bytes32_to_t_bytes32_fromStack(value0,  add(headStart, 0))

                abi_encode_t_bytes32_to_t_bytes32_fromStack(value1,  add(headStart, 32))

                abi_encode_t_bytes32_to_t_bytes32_fromStack(value2,  add(headStart, 64))

                abi_encode_t_uint256_to_t_uint256_fromStack(value3,  add(headStart, 96))

                abi_encode_t_address_to_t_address_fromStack(value4,  add(headStart, 128))

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

            function cleanup_t_address(value) -> cleaned {
                cleaned := cleanup_t_uint160(value)
            }

            function cleanup_t_bytes32(value) -> cleaned {
                cleaned := value
            }

            function cleanup_t_uint160(value) -> cleaned {
                cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffff)
            }

            function cleanup_t_uint256(value) -> cleaned {
                cleaned := value
            }

            /// @src 0:183,234
            function constant_name_1318() -> ret_mpos {
                /// @src 0:222,234
                let _3_mpos := convert_t_stringliteral_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738_to_t_string_memory_ptr()

                ret_mpos := _3_mpos
            }

            function constructor_IUniswapV2ERC20_2160() {

                /// @src 5:26,1147
                constructor_IUniswapV2Pair_1922()

            }

            function constructor_IUniswapV2Pair_1922() {

                /// @src 7:26,2468

            }

            function constructor_UniswapV2ERC20_1697() {

                /// @src 0:1004,1450
                constructor_IUniswapV2ERC20_2160()

                /// @src 0:1035,1047
                let var_chainId_1357
                let zero_t_uint256_2 := zero_value_for_split_t_uint256()
                var_chainId_1357 := zero_t_uint256_2
                /// @src 0:1057,1110
                { var_chainId_1357 := chainid() }
                /// @src 0:1189,1284
                let expr_1366 := 0x8b73c3c69bb8fe3d512ecc4cf759cc79239f7b179b0ffacaa9a75d522b39400f
                /// @src 0:1318,1322
                let expr_1370_mpos := constant_name_1318()
                /// @src 0:1312,1323
                let expr_1371_mpos := convert_array_t_string_memory_ptr_to_t_bytes_memory_ptr(expr_1370_mpos)
                /// @src 0:1302,1324
                let expr_1372 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_1371_mpos), array_length_t_bytes_memory_ptr(expr_1371_mpos))
                /// @src 0:1352,1362
                let expr_1377_mpos := convert_t_stringliteral_c89efdaa54c0f20c7adf612882df0950f5a951637e0307cdcb4c672f298b8bc6_to_t_bytes_memory_ptr()
                /// @src 0:1342,1363
                let expr_1378 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_1377_mpos), array_length_t_bytes_memory_ptr(expr_1377_mpos))
                /// @src 0:1381,1388
                let _4 := var_chainId_1357
                let expr_1379 := _4
                /// @src 0:1414,1418
                let expr_1382_address := address()
                /// @src 0:1406,1419
                let expr_1383 := convert_t_contract$_UniswapV2ERC20_$1697_to_t_address(expr_1382_address)
                /// @src 0:1161,1433

                let expr_1384_mpos := allocate_unbounded()
                let _5 := add(expr_1384_mpos, 0x20)

                let _6 := abi_encode_tuple_t_bytes32_t_bytes32_t_bytes32_t_uint256_t_address__to_t_bytes32_t_bytes32_t_bytes32_t_uint256_t_address__fromStack(_5, expr_1366, expr_1372, expr_1378, expr_1379, expr_1383)
                mstore(expr_1384_mpos, sub(_6, add(expr_1384_mpos, 0x20)))
                finalize_allocation(expr_1384_mpos, sub(_6, expr_1384_mpos))
                /// @src 0:1138,1443
                let expr_1385 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_1384_mpos), array_length_t_bytes_memory_ptr(expr_1384_mpos))
                /// @src 0:1119,1443
                update_storage_value_offset_0t_bytes32_to_t_bytes32(0x03, expr_1385)
                let expr_1386 := expr_1385

            }

            function constructor_UniswapV2Pair_1242() {

                /// @src 2:2300,2358
                constructor_UniswapV2ERC20_1697()
                /// @src 2:1202,1203
                let expr_257 := 0x01
                update_storage_value_offset_0t_rational_1_by_1_to_t_uint256(0x0c, expr_257)

                /// @src 2:2341,2351
                let expr_348 := caller()
                /// @src 2:2331,2351
                update_storage_value_offset_0t_address_to_t_address(0x05, expr_348)
                let expr_349 := expr_348

            }

            function convert_array_t_string_memory_ptr_to_t_bytes_memory_ptr(value) -> converted  {
                converted := value

            }

            function convert_t_address_to_t_address(value) -> converted {
                converted := convert_t_uint160_to_t_address(value)
            }

            function convert_t_bytes32_to_t_bytes32(value) -> converted {
                converted := cleanup_t_bytes32(value)
            }

            function convert_t_contract$_UniswapV2ERC20_$1697_to_t_address(value) -> converted {
                converted := convert_t_contract$_UniswapV2ERC20_$1697_to_t_uint160(value)
            }

            function convert_t_contract$_UniswapV2ERC20_$1697_to_t_uint160(value) -> converted {
                converted := cleanup_t_uint160(value)
            }

            function convert_t_rational_1_by_1_to_t_uint256(value) -> converted {
                converted := cleanup_t_uint256(value)
            }

            function convert_t_stringliteral_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738_to_t_string_memory_ptr() -> converted {
                converted := copy_literal_to_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738()
            }

            function convert_t_stringliteral_c89efdaa54c0f20c7adf612882df0950f5a951637e0307cdcb4c672f298b8bc6_to_t_bytes_memory_ptr() -> converted {
                converted := copy_literal_to_memory_c89efdaa54c0f20c7adf612882df0950f5a951637e0307cdcb4c672f298b8bc6()
            }

            function convert_t_uint160_to_t_address(value) -> converted {
                converted := convert_t_uint160_to_t_uint160(value)
            }

            function convert_t_uint160_to_t_uint160(value) -> converted {
                converted := cleanup_t_uint160(value)
            }

            function copy_literal_to_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738() -> memPtr {
                memPtr := allocate_memory_array_t_string_memory_ptr(10)
                store_literal_in_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738(add(memPtr, 32))
            }

            function copy_literal_to_memory_c89efdaa54c0f20c7adf612882df0950f5a951637e0307cdcb4c672f298b8bc6() -> memPtr {
                memPtr := allocate_memory_array_t_string_memory_ptr(1)
                store_literal_in_memory_c89efdaa54c0f20c7adf612882df0950f5a951637e0307cdcb4c672f298b8bc6(add(memPtr, 32))
            }

            function finalize_allocation(memPtr, size) {
                let newFreePtr := add(memPtr, round_up_to_mul_of_32(size))
                // protect against overflow
                if or(gt(newFreePtr, 0xffffffffffffffff), lt(newFreePtr, memPtr)) { panic_error_0x41() }
                mstore(64, newFreePtr)
            }

            function panic_error_0x41() {
                mstore(0, 35408467139433450592217433187231851964531694900788300625387963629091585785856)
                mstore(4, 0x41)
                revert(0, 0x24)
            }

            function prepare_store_t_address(value) -> ret {
                ret := value
            }

            function prepare_store_t_bytes32(value) -> ret {
                ret := shift_right_0_unsigned(value)
            }

            function prepare_store_t_uint256(value) -> ret {
                ret := value
            }

            function revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() {
                revert(0, 0)
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

            function store_literal_in_memory_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738(memPtr) {

                mstore(add(memPtr, 0), "Uniswap V2")

            }

            function store_literal_in_memory_c89efdaa54c0f20c7adf612882df0950f5a951637e0307cdcb4c672f298b8bc6(memPtr) {

                mstore(add(memPtr, 0), "1")

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

            function update_storage_value_offset_0t_address_to_t_address(slot, value_0) {
                let convertedValue_0 := convert_t_address_to_t_address(value_0)
                sstore(slot, update_byte_slice_20_shift_0(sload(slot), prepare_store_t_address(convertedValue_0)))
            }

            function update_storage_value_offset_0t_bytes32_to_t_bytes32(slot, value_0) {
                let convertedValue_0 := convert_t_bytes32_to_t_bytes32(value_0)
                sstore(slot, update_byte_slice_32_shift_0(sload(slot), prepare_store_t_bytes32(convertedValue_0)))
            }

            function update_storage_value_offset_0t_rational_1_by_1_to_t_uint256(slot, value_0) {
                let convertedValue_0 := convert_t_rational_1_by_1_to_t_uint256(value_0)
                sstore(slot, update_byte_slice_32_shift_0(sload(slot), prepare_store_t_uint256(convertedValue_0)))
            }

            function zero_value_for_split_t_uint256() -> ret {
                ret := 0
            }

        }
        object "UniswapV2Pair_1242_deployed" {
            code {
                /// @src 2:289,9957
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
                        fun_swap_1160(param_0, param_1, param_2, param_3, param_4)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x06fdde03
                    {
                        // name()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_name_1318()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_string_memory_ptr__to_t_string_memory_ptr__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x0902f1ac
                    {
                        // getReserves()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0, ret_1, ret_2 :=  fun_getReserves_300()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint112_t_uint112_t_uint32__to_t_uint112_t_uint112_t_uint32__fromStack(memPos , ret_0, ret_1, ret_2)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x095ea7b3
                    {
                        // approve(address,uint256)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0, param_1 :=  abi_decode_tuple_t_addresst_uint256(4, calldatasize())
                        let ret_0 :=  fun_approve_1541(param_0, param_1)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x0dfe1681
                    {
                        // token0()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_token0_237()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x18160ddd
                    {
                        // totalSupply()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_totalSupply_1329()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x23b872dd
                    {
                        // transferFrom(address,address,uint256)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0, param_1, param_2 :=  abi_decode_tuple_t_addresst_addresst_uint256(4, calldatasize())
                        let ret_0 :=  fun_transferFrom_1616(param_0, param_1, param_2)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x30adf81f
                    {
                        // PERMIT_TYPEHASH()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_PERMIT_TYPEHASH_1348()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_bytes32__to_t_bytes32__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x313ce567
                    {
                        // decimals()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_decimals_1326()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint8__to_t_uint8__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x3644e515
                    {
                        // DOMAIN_SEPARATOR()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_DOMAIN_SEPARATOR_1344()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_bytes32__to_t_bytes32__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x485cc955
                    {
                        // initialize(address,address)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0, param_1 :=  abi_decode_tuple_t_addresst_address(4, calldatasize())
                        fun_initialize_377(param_0, param_1)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x5909c0d5
                    {
                        // price0CumulativeLast()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_price0CumulativeLast_249()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x5a3d5493
                    {
                        // price1CumulativeLast()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_price1CumulativeLast_252()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x6a627842
                    {
                        // mint(address)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                        let ret_0 :=  fun_mint_750(param_0)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x70a08231
                    {
                        // balanceOf(address)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                        let ret_0 :=  getter_fun_balanceOf_1334(param_0)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x7464fc3d
                    {
                        // kLast()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_kLast_255()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x7ecebe00
                    {
                        // nonces(address)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                        let ret_0 :=  getter_fun_nonces_1353(param_0)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x89afcb44
                    {
                        // burn(address)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                        let ret_0, ret_1 :=  fun_burn_922(param_0)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256_t_uint256__to_t_uint256_t_uint256__fromStack(memPos , ret_0, ret_1)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0x95d89b41
                    {
                        // symbol()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_symbol_1322()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_string_memory_ptr__to_t_string_memory_ptr__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0xa9059cbb
                    {
                        // transfer(address,uint256)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0, param_1 :=  abi_decode_tuple_t_addresst_uint256(4, calldatasize())
                        let ret_0 :=  fun_transfer_1561(param_0, param_1)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0xba9a7a56
                    {
                        // MINIMUM_LIQUIDITY()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_MINIMUM_LIQUIDITY_220()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0xbc25cf77
                    {
                        // skim(address)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                        fun_skim_1211(param_0)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0xc45a0155
                    {
                        // factory()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_factory_234()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0xd21220a7
                    {
                        // token1()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        let ret_0 :=  getter_fun_token1_240()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_address__to_t_address__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0xd505accf
                    {
                        // permit(address,address,uint256,uint256,uint8,bytes32,bytes32)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0, param_1, param_2, param_3, param_4, param_5, param_6 :=  abi_decode_tuple_t_addresst_addresst_uint256t_uint256t_uint8t_bytes32t_bytes32(4, calldatasize())
                        fun_permit_1696(param_0, param_1, param_2, param_3, param_4, param_5, param_6)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0xdd62ed3e
                    {
                        // allowance(address,address)

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        let param_0, param_1 :=  abi_decode_tuple_t_addresst_address(4, calldatasize())
                        let ret_0 :=  getter_fun_allowance_1341(param_0, param_1)
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                        return(memPos, sub(memEnd, memPos))
                    }

                    case 0xfff6cae9
                    {
                        // sync()

                        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                        abi_decode_tuple_(4, calldatasize())
                        fun_sync_1241()
                        let memPos := allocate_unbounded()
                        let memEnd := abi_encode_tuple__to__fromStack(memPos  )
                        return(memPos, sub(memEnd, memPos))
                    }

                    default {}
                }
                if iszero(calldatasize()) {  }
                revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74()

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

                /// @src 2:416,471
                function constant_MINIMUM_LIQUIDITY_220() -> ret {
                    /// @src 2:466,471
                    let expr_219 := 0x03e8
                    let _5 := convert_t_rational_1000_by_1_to_t_uint256(expr_219)

                    ret := _5
                }

                /// @src 0:666,783
                function constant_PERMIT_TYPEHASH_1348() -> ret {
                    /// @src 0:717,783
                    let expr_1347 := 0x6e71edae12b1b97f4d1f60370fef10105fa2faae0126114a169c64845d6126c9
                    let _2 := convert_t_rational_49955707469362902507454157297736832118868343942642399513960811609542965143241_by_1_to_t_bytes32(expr_1347)

                    ret := _2
                }

                /// @src 10:206,236
                function constant_Q112_2005() -> ret {
                    /// @src 10:230,236
                    let expr_2004 := 0x010000000000000000000000000000
                    let _442 := convert_t_rational_5192296858534827628530496329220096_by_1_to_t_uint224(expr_2004)

                    ret := _442
                }

                /// @src 2:477,565
                function constant_SELECTOR_231() -> ret {
                    /// @src 2:529,563
                    let expr_228_mpos := convert_t_stringliteral_a9059cbb2ab09eb219583f4a59a5d0623ade346d962bcd4e46b11da047c9049b_to_t_bytes_memory_ptr()
                    /// @src 2:519,564
                    let expr_229 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_228_mpos), array_length_t_bytes_memory_ptr(expr_228_mpos))
                    /// @src 2:512,565
                    let expr_230 := convert_t_bytes32_to_t_bytes4(expr_229)
                    let _177 := expr_230

                    ret := _177
                }

                /// @src 0:295,339
                function constant_decimals_1326() -> ret {
                    /// @src 0:337,339
                    let expr_1325 := 0x12
                    let _3 := convert_t_rational_18_by_1_to_t_uint8(expr_1325)

                    ret := _3
                }

                /// @src 0:183,234
                function constant_name_1318() -> ret_mpos {
                    /// @src 0:222,234
                    let _1_mpos := convert_t_stringliteral_bfcc8ef98ffbf7b6c3fec7bf5185b566b9863e35a9d83acd49ad6824b5969738_to_t_string_memory_ptr()

                    ret_mpos := _1_mpos
                }

                /// @src 0:240,289
                function constant_symbol_1322() -> ret_mpos {
                    /// @src 0:281,289
                    let _4_mpos := convert_t_stringliteral_0c49a525f6758cfb27d0ada1467d2a2e99733995423d47ae30ae4ba2ba563255_to_t_string_memory_ptr()

                    ret_mpos := _4_mpos
                }

                function convert_t_address_to_t_address(value) -> converted {
                    converted := convert_t_uint160_to_t_address(value)
                }

                function convert_t_address_to_t_contract$_IERC20_$1781(value) -> converted {
                    converted := convert_t_uint160_to_t_contract$_IERC20_$1781(value)
                }

                function convert_t_address_to_t_contract$_IUniswapV2Callee_$1795(value) -> converted {
                    converted := convert_t_uint160_to_t_contract$_IUniswapV2Callee_$1795(value)
                }

                function convert_t_address_to_t_contract$_IUniswapV2Factory_$1305(value) -> converted {
                    converted := convert_t_uint160_to_t_contract$_IUniswapV2Factory_$1305(value)
                }

                function convert_t_bytes32_to_t_bytes4(value) -> converted {
                    converted := cleanup_t_bytes32(value)
                }

                function convert_t_contract$_IERC20_$1781_to_t_address(value) -> converted {
                    converted := convert_t_contract$_IERC20_$1781_to_t_uint160(value)
                }

                function convert_t_contract$_IERC20_$1781_to_t_uint160(value) -> converted {
                    converted := cleanup_t_uint160(value)
                }

                function convert_t_contract$_IUniswapV2Callee_$1795_to_t_address(value) -> converted {
                    converted := convert_t_contract$_IUniswapV2Callee_$1795_to_t_uint160(value)
                }

                function convert_t_contract$_IUniswapV2Callee_$1795_to_t_uint160(value) -> converted {
                    converted := cleanup_t_uint160(value)
                }

                function convert_t_contract$_IUniswapV2Factory_$1305_to_t_address(value) -> converted {
                    converted := convert_t_contract$_IUniswapV2Factory_$1305_to_t_uint160(value)
                }

                function convert_t_contract$_IUniswapV2Factory_$1305_to_t_uint160(value) -> converted {
                    converted := cleanup_t_uint160(value)
                }

                function convert_t_contract$_UniswapV2Pair_$1242_to_t_address(value) -> converted {
                    converted := convert_t_contract$_UniswapV2Pair_$1242_to_t_uint160(value)
                }

                function convert_t_contract$_UniswapV2Pair_$1242_to_t_uint160(value) -> converted {
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

                function convert_t_uint160_to_t_contract$_IERC20_$1781(value) -> converted {
                    converted := cleanup_t_uint160(value)
                }

                function convert_t_uint160_to_t_contract$_IUniswapV2Callee_$1795(value) -> converted {
                    converted := cleanup_t_uint160(value)
                }

                function convert_t_uint160_to_t_contract$_IUniswapV2Factory_$1305(value) -> converted {
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

                function fun__approve_1483(var_owner_1461, var_spender_1463, var_value_1465) {
                    /// @src 0:1870,2036

                    /// @src 0:1978,1983
                    let _335 := var_value_1465
                    let expr_1473 := _335
                    /// @src 0:1950,1959
                    let _336 := 0x02
                    let expr_1468 := _336
                    /// @src 0:1960,1965
                    let _337 := var_owner_1461
                    let expr_1469 := _337
                    /// @src 0:1950,1966
                    let _338 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_1468,expr_1469)
                    let _339 := _338
                    let expr_1471 := _339
                    /// @src 0:1967,1974
                    let _340 := var_spender_1463
                    let expr_1470 := _340
                    /// @src 0:1950,1975
                    let _341 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1471,expr_1470)
                    /// @src 0:1950,1983
                    update_storage_value_offset_0t_uint256_to_t_uint256(_341, expr_1473)
                    let expr_1474 := expr_1473
                    /// @src 0:2007,2012
                    let _342 := var_owner_1461
                    let expr_1477 := _342
                    /// @src 0:2014,2021
                    let _343 := var_spender_1463
                    let expr_1478 := _343
                    /// @src 0:2023,2028
                    let _344 := var_value_1465
                    let expr_1479 := _344
                    /// @src 0:1998,2029
                    let _345 := 0x8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925
                    {
                        let _346 := allocate_unbounded()
                        let _347 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_346 , expr_1479)
                        log3(_346, sub(_347, _346) , _345, expr_1477, expr_1478)
                    }
                }

                function fun__burn_1459(var_from_1426, var_value_1428) {
                    /// @src 0:1659,1864

                    /// @src 0:1737,1746
                    let _317 := 0x01
                    let expr_1434 := _317
                    /// @src 0:1747,1751
                    let _318 := var_from_1426
                    let expr_1435 := _318
                    /// @src 0:1737,1752
                    let _319 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1434,expr_1435)
                    let _320 := read_from_storage_split_offset_0_t_uint256(_319)
                    let expr_1436 := _320
                    /// @src 0:1737,1756
                    let expr_1437_self := expr_1436
                    let expr_1437_functionIdentifier := 2206
                    /// @src 0:1757,1762
                    let _321 := var_value_1428
                    let expr_1438 := _321
                    /// @src 0:1737,1763
                    let expr_1439 := fun_sub_2206(expr_1437_self, expr_1438)
                    /// @src 0:1719,1728
                    let _322 := 0x01
                    let expr_1431 := _322
                    /// @src 0:1729,1733
                    let _323 := var_from_1426
                    let expr_1432 := _323
                    /// @src 0:1719,1734
                    let _324 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1431,expr_1432)
                    /// @src 0:1719,1763
                    update_storage_value_offset_0t_uint256_to_t_uint256(_324, expr_1439)
                    let expr_1440 := expr_1439
                    /// @src 0:1787,1798
                    let _325 := read_from_storage_split_offset_0_t_uint256(0x00)
                    let expr_1443 := _325
                    /// @src 0:1787,1802
                    let expr_1444_self := expr_1443
                    let expr_1444_functionIdentifier := 2206
                    /// @src 0:1803,1808
                    let _326 := var_value_1428
                    let expr_1445 := _326
                    /// @src 0:1787,1809
                    let expr_1446 := fun_sub_2206(expr_1444_self, expr_1445)
                    /// @src 0:1773,1809
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x00, expr_1446)
                    let expr_1447 := expr_1446
                    /// @src 0:1833,1837
                    let _327 := var_from_1426
                    let expr_1450 := _327
                    /// @src 0:1847,1848
                    let expr_1453 := 0x00
                    /// @src 0:1839,1849
                    let expr_1454 := convert_t_rational_0_by_1_to_t_address(expr_1453)
                    /// @src 0:1851,1856
                    let _328 := var_value_1428
                    let expr_1455 := _328
                    /// @src 0:1824,1857
                    let _329 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
                    {
                        let _330 := allocate_unbounded()
                        let _331 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_330 , expr_1455)
                        log3(_330, sub(_331, _330) , _329, expr_1450, expr_1454)
                    }
                }

                function fun__mintFee_600(var__reserve0_494, var__reserve1_496) -> var_feeOn_499 {
                    /// @src 2:3663,4482
                    /// @src 2:3736,3746
                    let zero_t_bool_86 := zero_value_for_split_t_bool()
                    var_feeOn_499 := zero_t_bool_86

                    /// @src 2:3792,3799
                    let _87 := read_from_storage_split_offset_0_t_address(0x05)
                    let expr_504 := _87
                    /// @src 2:3774,3800
                    let expr_505_address := convert_t_address_to_t_contract$_IUniswapV2Factory_$1305(expr_504)
                    /// @src 2:3774,3806
                    let expr_506_address := convert_t_contract$_IUniswapV2Factory_$1305_to_t_address(expr_505_address)
                    let expr_506_functionSelector := 0x017e7e58
                    /// @src 2:3774,3808
                    if iszero(extcodesize(expr_506_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _88 := allocate_unbounded()
                    mstore(_88, shift_left_224(expr_506_functionSelector))
                    let _89 := abi_encode_tuple__to__fromStack(add(_88, 4) )

                    let _90 := staticcall(gas(), expr_506_address,  _88, sub(_89, _88), _88, 32)

                    if iszero(_90) { revert_forward_1() }

                    let expr_507
                    if _90 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_88, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_507 :=  abi_decode_tuple_t_address_fromMemory(_88, add(_88, returndatasize()))
                    }
                    /// @src 2:3758,3808
                    let var_feeTo_502 := expr_507
                    /// @src 2:3826,3831
                    let _91 := var_feeTo_502
                    let expr_510 := _91
                    /// @src 2:3843,3844
                    let expr_513 := 0x00
                    /// @src 2:3835,3845
                    let expr_514 := convert_t_rational_0_by_1_to_t_address(expr_513)
                    /// @src 2:3826,3845
                    let expr_515 := iszero(eq(cleanup_t_address(expr_510), cleanup_t_address(expr_514)))
                    /// @src 2:3818,3845
                    var_feeOn_499 := expr_515
                    let expr_516 := expr_515
                    /// @src 2:3869,3874
                    let _92 := read_from_storage_split_offset_0_t_uint256(0x0b)
                    let expr_520 := _92
                    /// @src 2:3855,3874
                    let var__kLast_519 := expr_520
                    /// @src 2:3903,3908
                    let _93 := var_feeOn_499
                    let expr_522 := _93
                    /// @src 2:3899,4476
                    switch expr_522
                    case 0 {
                        /// @src 2:4429,4435
                        let _94 := var__kLast_519
                        let expr_589 := _94
                        /// @src 2:4439,4440
                        let expr_590 := 0x00
                        /// @src 2:4429,4440
                        let expr_591 := iszero(eq(cleanup_t_uint256(expr_589), convert_t_rational_0_by_1_to_t_uint256(expr_590)))
                        /// @src 2:4425,4476
                        if expr_591 {
                            /// @src 2:4464,4465
                            let expr_593 := 0x00
                            /// @src 2:4456,4465
                            let _95 := convert_t_rational_0_by_1_to_t_uint256(expr_593)
                            update_storage_value_offset_0t_uint256_to_t_uint256(0x0b, _95)
                            let expr_594 := _95
                            /// @src 2:4425,4476
                        }
                        /// @src 2:3899,4476
                    }
                    default {
                        /// @src 2:3928,3934
                        let _96 := var__kLast_519
                        let expr_523 := _96
                        /// @src 2:3938,3939
                        let expr_524 := 0x00
                        /// @src 2:3928,3939
                        let expr_525 := iszero(eq(cleanup_t_uint256(expr_523), convert_t_rational_0_by_1_to_t_uint256(expr_524)))
                        /// @src 2:3924,4409
                        if expr_525 {
                            /// @src 2:3972,3976
                            let expr_528_address := linkersymbol("libraries/Math.sol:Math")
                            /// @src 2:3972,3981
                            let expr_529_functionIdentifier := 1997
                            /// @src 2:3987,3996
                            let _97 := var__reserve0_494
                            let expr_532 := _97
                            /// @src 2:3982,3997
                            let expr_533 := convert_t_uint112_to_t_uint256(expr_532)
                            /// @src 2:3982,4001
                            let expr_534_self := expr_533
                            let expr_534_functionIdentifier := 2234
                            /// @src 2:4002,4011
                            let _98 := var__reserve1_496
                            let expr_535 := _98
                            /// @src 2:3982,4012
                            let _99 := convert_t_uint112_to_t_uint256(expr_535)
                            let expr_536 := fun_mul_2234(expr_534_self, _99)
                            /// @src 2:3972,4013
                            let expr_537 := fun_sqrt_1997(expr_536)
                            /// @src 2:3959,4013
                            let var_rootK_527 := expr_537
                            /// @src 2:4048,4052
                            let expr_541_address := linkersymbol("libraries/Math.sol:Math")
                            /// @src 2:4048,4057
                            let expr_542_functionIdentifier := 1997
                            /// @src 2:4058,4064
                            let _100 := var__kLast_519
                            let expr_543 := _100
                            /// @src 2:4048,4065
                            let expr_544 := fun_sqrt_1997(expr_543)
                            /// @src 2:4031,4065
                            let var_rootKLast_540 := expr_544
                            /// @src 2:4087,4092
                            let _101 := var_rootK_527
                            let expr_546 := _101
                            /// @src 2:4095,4104
                            let _102 := var_rootKLast_540
                            let expr_547 := _102
                            /// @src 2:4087,4104
                            let expr_548 := gt(cleanup_t_uint256(expr_546), cleanup_t_uint256(expr_547))
                            /// @src 2:4083,4395
                            if expr_548 {
                                /// @src 2:4145,4156
                                let _103 := read_from_storage_split_offset_0_t_uint256(0x00)
                                let expr_551 := _103
                                /// @src 2:4145,4160
                                let expr_552_self := expr_551
                                let expr_552_functionIdentifier := 2234
                                /// @src 2:4161,4166
                                let _104 := var_rootK_527
                                let expr_553 := _104
                                /// @src 2:4161,4170
                                let expr_554_self := expr_553
                                let expr_554_functionIdentifier := 2206
                                /// @src 2:4171,4180
                                let _105 := var_rootKLast_540
                                let expr_555 := _105
                                /// @src 2:4161,4181
                                let expr_556 := fun_sub_2206(expr_554_self, expr_555)
                                /// @src 2:4145,4182
                                let expr_557 := fun_mul_2234(expr_552_self, expr_556)
                                /// @src 2:4128,4182
                                let var_numerator_550 := expr_557
                                /// @src 2:4223,4228
                                let _106 := var_rootK_527
                                let expr_561 := _106
                                /// @src 2:4223,4232
                                let expr_562_self := expr_561
                                let expr_562_functionIdentifier := 2234
                                /// @src 2:4233,4234
                                let expr_563 := 0x05
                                /// @src 2:4223,4235
                                let _107 := convert_t_rational_5_by_1_to_t_uint256(expr_563)
                                let expr_564 := fun_mul_2234(expr_562_self, _107)
                                /// @src 2:4223,4239
                                let expr_565_self := expr_564
                                let expr_565_functionIdentifier := 2184
                                /// @src 2:4240,4249
                                let _108 := var_rootKLast_540
                                let expr_566 := _108
                                /// @src 2:4223,4250
                                let expr_567 := fun_add_2184(expr_565_self, expr_566)
                                /// @src 2:4204,4250
                                let var_denominator_560 := expr_567
                                /// @src 2:4289,4298
                                let _109 := var_numerator_550
                                let expr_571 := _109
                                /// @src 2:4301,4312
                                let _110 := var_denominator_560
                                let expr_572 := _110
                                /// @src 2:4289,4312
                                let expr_573 := checked_div_t_uint256(expr_571, expr_572)

                                /// @src 2:4272,4312
                                let var_liquidity_570 := expr_573
                                /// @src 2:4338,4347
                                let _111 := var_liquidity_570
                                let expr_575 := _111
                                /// @src 2:4350,4351
                                let expr_576 := 0x00
                                /// @src 2:4338,4351
                                let expr_577 := gt(cleanup_t_uint256(expr_575), convert_t_rational_0_by_1_to_t_uint256(expr_576))
                                /// @src 2:4334,4376
                                if expr_577 {
                                    /// @src 2:4353,4358
                                    let expr_578_functionIdentifier := 1424
                                    /// @src 2:4359,4364
                                    let _112 := var_feeTo_502
                                    let expr_579 := _112
                                    /// @src 2:4366,4375
                                    let _113 := var_liquidity_570
                                    let expr_580 := _113
                                    fun__mint_1424(expr_579, expr_580)
                                    /// @src 2:4334,4376
                                }
                                /// @src 2:4083,4395
                            }
                            /// @src 2:3924,4409
                        }
                        /// @src 2:3899,4476
                    }

                }

                function fun__mint_1424(var_to_1391, var_value_1393) {
                    /// @src 0:1456,1653

                    /// @src 0:1528,1539
                    let _302 := read_from_storage_split_offset_0_t_uint256(0x00)
                    let expr_1397 := _302
                    /// @src 0:1528,1543
                    let expr_1398_self := expr_1397
                    let expr_1398_functionIdentifier := 2184
                    /// @src 0:1544,1549
                    let _303 := var_value_1393
                    let expr_1399 := _303
                    /// @src 0:1528,1550
                    let expr_1400 := fun_add_2184(expr_1398_self, expr_1399)
                    /// @src 0:1514,1550
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x00, expr_1400)
                    let expr_1401 := expr_1400
                    /// @src 0:1576,1585
                    let _304 := 0x01
                    let expr_1406 := _304
                    /// @src 0:1586,1588
                    let _305 := var_to_1391
                    let expr_1407 := _305
                    /// @src 0:1576,1589
                    let _306 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1406,expr_1407)
                    let _307 := read_from_storage_split_offset_0_t_uint256(_306)
                    let expr_1408 := _307
                    /// @src 0:1576,1593
                    let expr_1409_self := expr_1408
                    let expr_1409_functionIdentifier := 2184
                    /// @src 0:1594,1599
                    let _308 := var_value_1393
                    let expr_1410 := _308
                    /// @src 0:1576,1600
                    let expr_1411 := fun_add_2184(expr_1409_self, expr_1410)
                    /// @src 0:1560,1569
                    let _309 := 0x01
                    let expr_1403 := _309
                    /// @src 0:1570,1572
                    let _310 := var_to_1391
                    let expr_1404 := _310
                    /// @src 0:1560,1573
                    let _311 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1403,expr_1404)
                    /// @src 0:1560,1600
                    update_storage_value_offset_0t_uint256_to_t_uint256(_311, expr_1411)
                    let expr_1412 := expr_1411
                    /// @src 0:1632,1633
                    let expr_1417 := 0x00
                    /// @src 0:1624,1634
                    let expr_1418 := convert_t_rational_0_by_1_to_t_address(expr_1417)
                    /// @src 0:1636,1638
                    let _312 := var_to_1391
                    let expr_1419 := _312
                    /// @src 0:1640,1645
                    let _313 := var_value_1393
                    let expr_1420 := _313
                    /// @src 0:1615,1646
                    let _314 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
                    {
                        let _315 := allocate_unbounded()
                        let _316 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_315 , expr_1420)
                        log3(_315, sub(_316, _315) , _314, expr_1418, expr_1419)
                    }
                }

                function fun__safeTransfer_343(var_token_302, var_to_304, var_value_306) {
                    /// @src 2:1588,1872

                    /// @src 2:1704,1709
                    let _176 := var_token_302
                    let expr_313 := _176
                    /// @src 2:1704,1714
                    let expr_314_address := expr_313
                    /// @src 2:1738,1746
                    let expr_317 := constant_SELECTOR_231()
                    /// @src 2:1748,1750
                    let _178 := var_to_304
                    let expr_318 := _178
                    /// @src 2:1752,1757
                    let _179 := var_value_306
                    let expr_319 := _179
                    /// @src 2:1715,1758

                    let expr_320_mpos := allocate_unbounded()
                    let _180 := add(expr_320_mpos, 0x20)

                    mstore(_180, expr_317)
                    _180 := add(_180, 4)

                    let _181 := abi_encode_tuple_t_address_t_uint256__to_t_address_t_uint256__fromStack(_180, expr_318, expr_319)
                    mstore(expr_320_mpos, sub(_181, add(expr_320_mpos, 0x20)))
                    finalize_allocation(expr_320_mpos, sub(_181, expr_320_mpos))
                    /// @src 2:1704,1759

                    let _182 := add(expr_320_mpos, 0x20)
                    let _183 := mload(expr_320_mpos)

                    let expr_321_component_1 := call(gas(), expr_314_address,  0,  _182, _183, 0, 0)
                    let expr_321_component_2_mpos := extract_returndata()
                    /// @src 2:1668,1759
                    let var_success_310 := expr_321_component_1
                    let var_data_312_mpos := expr_321_component_2_mpos
                    /// @src 2:1777,1784
                    let _184 := var_success_310
                    let expr_324 := _184
                    /// @src 2:1777,1834
                    let expr_338 := expr_324
                    if expr_338 {
                        /// @src 2:1789,1793
                        let _185_mpos := var_data_312_mpos
                        let expr_325_mpos := _185_mpos
                        /// @src 2:1789,1800
                        let expr_326 := array_length_t_bytes_memory_ptr(expr_325_mpos)
                        /// @src 2:1804,1805
                        let expr_327 := 0x00
                        /// @src 2:1789,1805
                        let expr_328 := eq(cleanup_t_uint256(expr_326), convert_t_rational_0_by_1_to_t_uint256(expr_327))
                        /// @src 2:1789,1833
                        let expr_336 := expr_328
                        if iszero(expr_336) {
                            /// @src 2:1820,1824
                            let _186_mpos := var_data_312_mpos
                            let expr_331_mpos := _186_mpos
                            /// @src 2:1809,1833

                            let expr_335 :=  abi_decode_tuple_t_bool_fromMemory(add(expr_331_mpos, 32), add(add(expr_331_mpos, 32), array_length_t_bytes_memory_ptr(expr_331_mpos)))
                            /// @src 2:1789,1833
                            expr_336 := expr_335
                        }
                        /// @src 2:1788,1834
                        let expr_337 := expr_336
                        /// @src 2:1777,1834
                        expr_338 := expr_337
                    }
                    /// @src 2:1769,1865
                    require_helper_t_stringliteral_d8733df98393ec23d211b1de22ecb14bb9ce352e147cbbbe19c11e12e90b7ff2(expr_338)

                }

                function fun__transfer_1521(var_from_1485, var_to_1487, var_value_1489) {
                    /// @src 0:2042,2258

                    /// @src 0:2135,2144
                    let _351 := 0x01
                    let expr_1495 := _351
                    /// @src 0:2145,2149
                    let _352 := var_from_1485
                    let expr_1496 := _352
                    /// @src 0:2135,2150
                    let _353 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1495,expr_1496)
                    let _354 := read_from_storage_split_offset_0_t_uint256(_353)
                    let expr_1497 := _354
                    /// @src 0:2135,2154
                    let expr_1498_self := expr_1497
                    let expr_1498_functionIdentifier := 2206
                    /// @src 0:2155,2160
                    let _355 := var_value_1489
                    let expr_1499 := _355
                    /// @src 0:2135,2161
                    let expr_1500 := fun_sub_2206(expr_1498_self, expr_1499)
                    /// @src 0:2117,2126
                    let _356 := 0x01
                    let expr_1492 := _356
                    /// @src 0:2127,2131
                    let _357 := var_from_1485
                    let expr_1493 := _357
                    /// @src 0:2117,2132
                    let _358 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1492,expr_1493)
                    /// @src 0:2117,2161
                    update_storage_value_offset_0t_uint256_to_t_uint256(_358, expr_1500)
                    let expr_1501 := expr_1500
                    /// @src 0:2187,2196
                    let _359 := 0x01
                    let expr_1506 := _359
                    /// @src 0:2197,2199
                    let _360 := var_to_1487
                    let expr_1507 := _360
                    /// @src 0:2187,2200
                    let _361 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1506,expr_1507)
                    let _362 := read_from_storage_split_offset_0_t_uint256(_361)
                    let expr_1508 := _362
                    /// @src 0:2187,2204
                    let expr_1509_self := expr_1508
                    let expr_1509_functionIdentifier := 2184
                    /// @src 0:2205,2210
                    let _363 := var_value_1489
                    let expr_1510 := _363
                    /// @src 0:2187,2211
                    let expr_1511 := fun_add_2184(expr_1509_self, expr_1510)
                    /// @src 0:2171,2180
                    let _364 := 0x01
                    let expr_1503 := _364
                    /// @src 0:2181,2183
                    let _365 := var_to_1487
                    let expr_1504 := _365
                    /// @src 0:2171,2184
                    let _366 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1503,expr_1504)
                    /// @src 0:2171,2211
                    update_storage_value_offset_0t_uint256_to_t_uint256(_366, expr_1511)
                    let expr_1512 := expr_1511
                    /// @src 0:2235,2239
                    let _367 := var_from_1485
                    let expr_1515 := _367
                    /// @src 0:2241,2243
                    let _368 := var_to_1487
                    let expr_1516 := _368
                    /// @src 0:2245,2250
                    let _369 := var_value_1489
                    let expr_1517 := _369
                    /// @src 0:2226,2251
                    let _370 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
                    {
                        let _371 := allocate_unbounded()
                        let _372 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_371 , expr_1517)
                        log3(_371, sub(_372, _371) , _370, expr_1515, expr_1516)
                    }
                }

                function fun__update_492(var_balance0_379, var_balance1_381, var__reserve0_383, var__reserve1_385) {
                    /// @src 2:2717,3576

                    /// @src 2:2828,2836
                    let _63 := var_balance0_379
                    let expr_389 := _63
                    /// @src 2:2840,2857
                    let expr_394 := 0xffffffffffffffffffffffffffff
                    /// @src 2:2828,2857
                    let expr_395 := iszero(gt(cleanup_t_uint256(expr_389), convert_t_uint112_to_t_uint256(expr_394)))
                    /// @src 2:2828,2890
                    let expr_403 := expr_395
                    if expr_403 {
                        /// @src 2:2861,2869
                        let _64 := var_balance1_381
                        let expr_396 := _64
                        /// @src 2:2873,2890
                        let expr_401 := 0xffffffffffffffffffffffffffff
                        /// @src 2:2861,2890
                        let expr_402 := iszero(gt(cleanup_t_uint256(expr_396), convert_t_uint112_to_t_uint256(expr_401)))
                        /// @src 2:2828,2890
                        expr_403 := expr_402
                    }
                    /// @src 2:2820,2914
                    require_helper_t_stringliteral_a5d1f08cd66a1a59e841a286c7f2c877311b5d331d2315cd2fe3c5f05e833928(expr_403)
                    /// @src 2:2955,2970
                    let expr_412 := timestamp()
                    /// @src 2:2973,2978
                    let expr_415 := 0x0100000000
                    /// @src 2:2955,2978
                    let expr_416 := mod_t_uint256(expr_412, convert_t_rational_4294967296_by_1_to_t_uint256(expr_415))

                    /// @src 2:2948,2979
                    let expr_417 := convert_t_uint256_to_t_uint32(expr_416)
                    /// @src 2:2924,2979
                    let var_blockTimestamp_408 := expr_417
                    /// @src 2:3010,3024
                    let _65 := var_blockTimestamp_408
                    let expr_421 := _65
                    /// @src 2:3027,3045
                    let _66 := read_from_storage_split_offset_28_t_uint32(0x08)
                    let expr_422 := _66
                    /// @src 2:3010,3045
                    let expr_423 := checked_sub_t_uint32(expr_421, expr_422)

                    /// @src 2:2989,3045
                    let var_timeElapsed_420 := expr_423
                    /// @src 2:3082,3093
                    let _67 := var_timeElapsed_420
                    let expr_425 := _67
                    /// @src 2:3096,3097
                    let expr_426 := 0x00
                    /// @src 2:3082,3097
                    let expr_427 := gt(cleanup_t_uint32(expr_425), convert_t_rational_0_by_1_to_t_uint32(expr_426))
                    /// @src 2:3082,3115
                    let expr_431 := expr_427
                    if expr_431 {
                        /// @src 2:3101,3110
                        let _68 := var__reserve0_383
                        let expr_428 := _68
                        /// @src 2:3114,3115
                        let expr_429 := 0x00
                        /// @src 2:3101,3115
                        let expr_430 := iszero(eq(cleanup_t_uint112(expr_428), convert_t_rational_0_by_1_to_t_uint112(expr_429)))
                        /// @src 2:3082,3115
                        expr_431 := expr_430
                    }
                    /// @src 2:3082,3133
                    let expr_435 := expr_431
                    if expr_435 {
                        /// @src 2:3119,3128
                        let _69 := var__reserve1_385
                        let expr_432 := _69
                        /// @src 2:3132,3133
                        let expr_433 := 0x00
                        /// @src 2:3119,3133
                        let expr_434 := iszero(eq(cleanup_t_uint112(expr_432), convert_t_rational_0_by_1_to_t_uint112(expr_433)))
                        /// @src 2:3082,3133
                        expr_435 := expr_434
                    }
                    /// @src 2:3078,3410
                    if expr_435 {
                        /// @src 2:3238,3247
                        let expr_439_address := linkersymbol("libraries/UQ112x112.sol:UQ112x112")
                        /// @src 2:3238,3254
                        let expr_440_functionIdentifier := 2022
                        /// @src 2:3255,3264
                        let _70 := var__reserve1_385
                        let expr_441 := _70
                        /// @src 2:3238,3265
                        let expr_442 := fun_encode_2022(expr_441)
                        /// @src 2:3238,3271
                        let expr_443_self := expr_442
                        let expr_443_functionIdentifier := 2041
                        /// @src 2:3272,3281
                        let _71 := var__reserve0_383
                        let expr_444 := _71
                        /// @src 2:3238,3282
                        let expr_445 := fun_uqdiv_2041(expr_443_self, expr_444)
                        /// @src 2:3233,3283
                        let expr_446 := convert_t_uint224_to_t_uint256(expr_445)
                        /// @src 2:3286,3297
                        let _72 := var_timeElapsed_420
                        let expr_447 := _72
                        /// @src 2:3233,3297
                        let expr_448 := checked_mul_t_uint256(expr_446, convert_t_uint32_to_t_uint256(expr_447))

                        /// @src 2:3209,3297
                        let _73 := read_from_storage_split_offset_0_t_uint256(0x09)
                        let expr_449 := checked_add_t_uint256(_73, expr_448)

                        update_storage_value_offset_0t_uint256_to_t_uint256(0x09, expr_449)
                        /// @src 2:3340,3349
                        let expr_454_address := linkersymbol("libraries/UQ112x112.sol:UQ112x112")
                        /// @src 2:3340,3356
                        let expr_455_functionIdentifier := 2022
                        /// @src 2:3357,3366
                        let _74 := var__reserve0_383
                        let expr_456 := _74
                        /// @src 2:3340,3367
                        let expr_457 := fun_encode_2022(expr_456)
                        /// @src 2:3340,3373
                        let expr_458_self := expr_457
                        let expr_458_functionIdentifier := 2041
                        /// @src 2:3374,3383
                        let _75 := var__reserve1_385
                        let expr_459 := _75
                        /// @src 2:3340,3384
                        let expr_460 := fun_uqdiv_2041(expr_458_self, expr_459)
                        /// @src 2:3335,3385
                        let expr_461 := convert_t_uint224_to_t_uint256(expr_460)
                        /// @src 2:3388,3399
                        let _76 := var_timeElapsed_420
                        let expr_462 := _76
                        /// @src 2:3335,3399
                        let expr_463 := checked_mul_t_uint256(expr_461, convert_t_uint32_to_t_uint256(expr_462))

                        /// @src 2:3311,3399
                        let _77 := read_from_storage_split_offset_0_t_uint256(0x0a)
                        let expr_464 := checked_add_t_uint256(_77, expr_463)

                        update_storage_value_offset_0t_uint256_to_t_uint256(0x0a, expr_464)
                        /// @src 2:3078,3410
                    }
                    /// @src 2:3438,3446
                    let _78 := var_balance0_379
                    let expr_471 := _78
                    /// @src 2:3430,3447
                    let expr_472 := convert_t_uint256_to_t_uint112(expr_471)
                    /// @src 2:3419,3447
                    update_storage_value_offset_0t_uint112_to_t_uint112(0x08, expr_472)
                    let expr_473 := expr_472
                    /// @src 2:3476,3484
                    let _79 := var_balance1_381
                    let expr_478 := _79
                    /// @src 2:3468,3485
                    let expr_479 := convert_t_uint256_to_t_uint112(expr_478)
                    /// @src 2:3457,3485
                    update_storage_value_offset_14t_uint112_to_t_uint112(0x08, expr_479)
                    let expr_480 := expr_479
                    /// @src 2:3516,3530
                    let _80 := var_blockTimestamp_408
                    let expr_483 := _80
                    /// @src 2:3495,3530
                    update_storage_value_offset_28t_uint32_to_t_uint32(0x08, expr_483)
                    let expr_484 := expr_483
                    /// @src 2:3550,3558
                    let _81 := read_from_storage_split_offset_0_t_uint112(0x08)
                    let expr_487 := _81
                    /// @src 2:3560,3568
                    let _82 := read_from_storage_split_offset_14_t_uint112(0x08)
                    let expr_488 := _82
                    /// @src 2:3545,3569
                    let _83 := 0x1c411e9a96e071241c2f21f7726b17ae89e3cab4c78be50e062b03a9fffbbad1
                    {
                        let _84 := allocate_unbounded()
                        let _85 := abi_encode_tuple_t_uint112_t_uint112__to_t_uint112_t_uint112__fromStack(_84 , expr_487, expr_488)
                        log1(_84, sub(_85, _84) , _83)
                    }
                }

                function fun_add_2184(var_x_2164, var_y_2166) -> var_z_2169 {
                    /// @src 9:154,280
                    /// @src 9:206,212
                    let zero_t_uint256_446 := zero_value_for_split_t_uint256()
                    var_z_2169 := zero_t_uint256_446

                    /// @src 9:237,238
                    let _447 := var_x_2164
                    let expr_2173 := _447
                    /// @src 9:241,242
                    let _448 := var_y_2166
                    let expr_2174 := _448
                    /// @src 9:237,242
                    let expr_2175 := checked_add_t_uint256(expr_2173, expr_2174)

                    /// @src 9:233,242
                    var_z_2169 := expr_2175
                    let expr_2176 := expr_2175
                    /// @src 9:232,243
                    let expr_2177 := expr_2176
                    /// @src 9:247,248
                    let _449 := var_x_2164
                    let expr_2178 := _449
                    /// @src 9:232,248
                    let expr_2179 := iszero(lt(cleanup_t_uint256(expr_2177), cleanup_t_uint256(expr_2178)))
                    /// @src 9:224,273
                    require_helper_t_stringliteral_3903056b84ed2aba2be78662dc6c5c99b160cebe9af9bd9493d0fc28ff16f6db(expr_2179)

                }

                function fun_approve_1541(var_spender_1523, var_value_1525) -> var__1529 {
                    /// @src 0:2264,2417
                    /// @src 0:2337,2341
                    let zero_t_bool_332 := zero_value_for_split_t_bool()
                    var__1529 := zero_t_bool_332

                    /// @src 0:2353,2361
                    let expr_1531_functionIdentifier := 1483
                    /// @src 0:2362,2372
                    let expr_1533 := caller()
                    /// @src 0:2374,2381
                    let _333 := var_spender_1523
                    let expr_1534 := _333
                    /// @src 0:2383,2388
                    let _334 := var_value_1525
                    let expr_1535 := _334
                    fun__approve_1483(expr_1533, expr_1534, expr_1535)
                    /// @src 0:2406,2410
                    let expr_1538 := 0x01
                    /// @src 0:2399,2410
                    var__1529 := expr_1538
                    leave

                }

                function fun_burn_922(var_to_752) -> var_amount0_758, var_amount1_760 {
                    /// @src 2:5928,7381
                    /// @src 2:5986,5998
                    let zero_t_uint256_114 := zero_value_for_split_t_uint256()
                    var_amount0_758 := zero_t_uint256_114
                    /// @src 2:6000,6012
                    let zero_t_uint256_115 := zero_value_for_split_t_uint256()
                    var_amount1_760 := zero_t_uint256_115

                    var_amount0_758, var_amount1_760 := modifier_lock_756(var_amount0_758, var_amount1_760, var_to_752)
                }

                function fun_burn_922_inner(_121, _122, var_to_752) -> var_amount0_758, var_amount1_760 {
                    /// @src 2:5928,7381
                    var_amount1_760 := _121
                    var_amount1_760 := _122

                    /// @src 2:6066,6077
                    let expr_766_functionIdentifier := 300
                    /// @src 2:6066,6079
                    let expr_767_component_1, expr_767_component_2, expr_767_component_3 := fun_getReserves_300()
                    /// @src 2:6024,6079
                    let var__reserve0_763 := expr_767_component_1
                    let var__reserve1_765 := expr_767_component_2
                    /// @src 2:6122,6128
                    let _123 := read_from_storage_split_offset_0_t_address(0x06)
                    let expr_771 := _123
                    /// @src 2:6104,6128
                    let var__token0_770 := expr_771
                    /// @src 2:6202,6208
                    let _124 := read_from_storage_split_offset_0_t_address(0x07)
                    let expr_775 := _124
                    /// @src 2:6184,6208
                    let var__token1_774 := expr_775
                    /// @src 2:6287,6294
                    let _125 := var__token0_770
                    let expr_780 := _125
                    /// @src 2:6280,6295
                    let expr_781_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_780)
                    /// @src 2:6280,6305
                    let expr_782_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_781_address)
                    let expr_782_functionSelector := 0x70a08231
                    /// @src 2:6314,6318
                    let expr_785_address := address()
                    /// @src 2:6306,6319
                    let expr_786 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_785_address)
                    /// @src 2:6280,6320
                    if iszero(extcodesize(expr_782_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _126 := allocate_unbounded()
                    mstore(_126, shift_left_224(expr_782_functionSelector))
                    let _127 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_126, 4) , expr_786)

                    let _128 := staticcall(gas(), expr_782_address,  _126, sub(_127, _126), _126, 32)

                    if iszero(_128) { revert_forward_1() }

                    let expr_787
                    if _128 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_126, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_787 :=  abi_decode_tuple_t_uint256_fromMemory(_126, add(_126, returndatasize()))
                    }
                    /// @src 2:6264,6320
                    let var_balance0_778 := expr_787
                    /// @src 2:6353,6360
                    let _129 := var__token1_774
                    let expr_792 := _129
                    /// @src 2:6346,6361
                    let expr_793_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_792)
                    /// @src 2:6346,6371
                    let expr_794_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_793_address)
                    let expr_794_functionSelector := 0x70a08231
                    /// @src 2:6380,6384
                    let expr_797_address := address()
                    /// @src 2:6372,6385
                    let expr_798 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_797_address)
                    /// @src 2:6346,6386
                    if iszero(extcodesize(expr_794_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _130 := allocate_unbounded()
                    mstore(_130, shift_left_224(expr_794_functionSelector))
                    let _131 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_130, 4) , expr_798)

                    let _132 := staticcall(gas(), expr_794_address,  _130, sub(_131, _130), _130, 32)

                    if iszero(_132) { revert_forward_1() }

                    let expr_799
                    if _132 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_130, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_799 :=  abi_decode_tuple_t_uint256_fromMemory(_130, add(_130, returndatasize()))
                    }
                    /// @src 2:6330,6386
                    let var_balance1_790 := expr_799
                    /// @src 2:6413,6422
                    let _133 := 0x01
                    let expr_803 := _133
                    /// @src 2:6431,6435
                    let expr_806_address := address()
                    /// @src 2:6423,6436
                    let expr_807 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_806_address)
                    /// @src 2:6413,6437
                    let _134 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_803,expr_807)
                    let _135 := read_from_storage_split_offset_0_t_uint256(_134)
                    let expr_808 := _135
                    /// @src 2:6396,6437
                    let var_liquidity_802 := expr_808
                    /// @src 2:6461,6469
                    let expr_812_functionIdentifier := 600
                    /// @src 2:6470,6479
                    let _136 := var__reserve0_763
                    let expr_813 := _136
                    /// @src 2:6481,6490
                    let _137 := var__reserve1_765
                    let expr_814 := _137
                    /// @src 2:6461,6491
                    let expr_815 := fun__mintFee_600(expr_813, expr_814)
                    /// @src 2:6448,6491
                    let var_feeOn_811 := expr_815
                    /// @src 2:6521,6532
                    let _138 := read_from_storage_split_offset_0_t_uint256(0x00)
                    let expr_819 := _138
                    /// @src 2:6501,6532
                    let var__totalSupply_818 := expr_819
                    /// @src 2:6630,6639
                    let _139 := var_liquidity_802
                    let expr_822 := _139
                    /// @src 2:6630,6643
                    let expr_823_self := expr_822
                    let expr_823_functionIdentifier := 2234
                    /// @src 2:6644,6652
                    let _140 := var_balance0_778
                    let expr_824 := _140
                    /// @src 2:6630,6653
                    let expr_825 := fun_mul_2234(expr_823_self, expr_824)
                    /// @src 2:6656,6668
                    let _141 := var__totalSupply_818
                    let expr_826 := _141
                    /// @src 2:6630,6668
                    let expr_827 := checked_div_t_uint256(expr_825, expr_826)

                    /// @src 2:6620,6668
                    var_amount0_758 := expr_827
                    let expr_828 := expr_827
                    /// @src 2:6736,6745
                    let _142 := var_liquidity_802
                    let expr_831 := _142
                    /// @src 2:6736,6749
                    let expr_832_self := expr_831
                    let expr_832_functionIdentifier := 2234
                    /// @src 2:6750,6758
                    let _143 := var_balance1_790
                    let expr_833 := _143
                    /// @src 2:6736,6759
                    let expr_834 := fun_mul_2234(expr_832_self, expr_833)
                    /// @src 2:6762,6774
                    let _144 := var__totalSupply_818
                    let expr_835 := _144
                    /// @src 2:6736,6774
                    let expr_836 := checked_div_t_uint256(expr_834, expr_835)

                    /// @src 2:6726,6774
                    var_amount1_760 := expr_836
                    let expr_837 := expr_836
                    /// @src 2:6840,6847
                    let _145 := var_amount0_758
                    let expr_840 := _145
                    /// @src 2:6850,6851
                    let expr_841 := 0x00
                    /// @src 2:6840,6851
                    let expr_842 := gt(cleanup_t_uint256(expr_840), convert_t_rational_0_by_1_to_t_uint256(expr_841))
                    /// @src 2:6840,6866
                    let expr_846 := expr_842
                    if expr_846 {
                        /// @src 2:6855,6862
                        let _146 := var_amount1_760
                        let expr_843 := _146
                        /// @src 2:6865,6866
                        let expr_844 := 0x00
                        /// @src 2:6855,6866
                        let expr_845 := gt(cleanup_t_uint256(expr_843), convert_t_rational_0_by_1_to_t_uint256(expr_844))
                        /// @src 2:6840,6866
                        expr_846 := expr_845
                    }
                    /// @src 2:6832,6911
                    require_helper_t_stringliteral_57ebfb4dd8b5082cdba0f23c17077e3b0ecb9782a51e0e9a15e9bc8c4b30c562(expr_846)
                    /// @src 2:6921,6926
                    let expr_850_functionIdentifier := 1459
                    /// @src 2:6935,6939
                    let expr_853_address := address()
                    /// @src 2:6927,6940
                    let expr_854 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_853_address)
                    /// @src 2:6942,6951
                    let _147 := var_liquidity_802
                    let expr_855 := _147
                    fun__burn_1459(expr_854, expr_855)
                    /// @src 2:6962,6975
                    let expr_858_functionIdentifier := 343
                    /// @src 2:6976,6983
                    let _148 := var__token0_770
                    let expr_859 := _148
                    /// @src 2:6985,6987
                    let _149 := var_to_752
                    let expr_860 := _149
                    /// @src 2:6989,6996
                    let _150 := var_amount0_758
                    let expr_861 := _150
                    fun__safeTransfer_343(expr_859, expr_860, expr_861)
                    /// @src 2:7007,7020
                    let expr_864_functionIdentifier := 343
                    /// @src 2:7021,7028
                    let _151 := var__token1_774
                    let expr_865 := _151
                    /// @src 2:7030,7032
                    let _152 := var_to_752
                    let expr_866 := _152
                    /// @src 2:7034,7041
                    let _153 := var_amount1_760
                    let expr_867 := _153
                    fun__safeTransfer_343(expr_865, expr_866, expr_867)
                    /// @src 2:7070,7077
                    let _154 := var__token0_770
                    let expr_872 := _154
                    /// @src 2:7063,7078
                    let expr_873_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_872)
                    /// @src 2:7063,7088
                    let expr_874_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_873_address)
                    let expr_874_functionSelector := 0x70a08231
                    /// @src 2:7097,7101
                    let expr_877_address := address()
                    /// @src 2:7089,7102
                    let expr_878 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_877_address)
                    /// @src 2:7063,7103
                    if iszero(extcodesize(expr_874_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _155 := allocate_unbounded()
                    mstore(_155, shift_left_224(expr_874_functionSelector))
                    let _156 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_155, 4) , expr_878)

                    let _157 := staticcall(gas(), expr_874_address,  _155, sub(_156, _155), _155, 32)

                    if iszero(_157) { revert_forward_1() }

                    let expr_879
                    if _157 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_155, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_879 :=  abi_decode_tuple_t_uint256_fromMemory(_155, add(_155, returndatasize()))
                    }
                    /// @src 2:7052,7103
                    var_balance0_778 := expr_879
                    let expr_880 := expr_879
                    /// @src 2:7131,7138
                    let _158 := var__token1_774
                    let expr_884 := _158
                    /// @src 2:7124,7139
                    let expr_885_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_884)
                    /// @src 2:7124,7149
                    let expr_886_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_885_address)
                    let expr_886_functionSelector := 0x70a08231
                    /// @src 2:7158,7162
                    let expr_889_address := address()
                    /// @src 2:7150,7163
                    let expr_890 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_889_address)
                    /// @src 2:7124,7164
                    if iszero(extcodesize(expr_886_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _159 := allocate_unbounded()
                    mstore(_159, shift_left_224(expr_886_functionSelector))
                    let _160 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_159, 4) , expr_890)

                    let _161 := staticcall(gas(), expr_886_address,  _159, sub(_160, _159), _159, 32)

                    if iszero(_161) { revert_forward_1() }

                    let expr_891
                    if _161 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_159, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_891 :=  abi_decode_tuple_t_uint256_fromMemory(_159, add(_159, returndatasize()))
                    }
                    /// @src 2:7113,7164
                    var_balance1_790 := expr_891
                    let expr_892 := expr_891
                    /// @src 2:7175,7182
                    let expr_894_functionIdentifier := 492
                    /// @src 2:7183,7191
                    let _162 := var_balance0_778
                    let expr_895 := _162
                    /// @src 2:7193,7201
                    let _163 := var_balance1_790
                    let expr_896 := _163
                    /// @src 2:7203,7212
                    let _164 := var__reserve0_763
                    let expr_897 := _164
                    /// @src 2:7214,7223
                    let _165 := var__reserve1_765
                    let expr_898 := _165
                    fun__update_492(expr_895, expr_896, expr_897, expr_898)
                    /// @src 2:7238,7243
                    let _166 := var_feeOn_811
                    let expr_901 := _166
                    /// @src 2:7234,7281
                    if expr_901 {
                        /// @src 2:7258,7266
                        let _167 := read_from_storage_split_offset_0_t_uint112(0x08)
                        let expr_905 := _167
                        /// @src 2:7253,7267
                        let expr_906 := convert_t_uint112_to_t_uint256(expr_905)
                        /// @src 2:7253,7271
                        let expr_907_self := expr_906
                        let expr_907_functionIdentifier := 2234
                        /// @src 2:7272,7280
                        let _168 := read_from_storage_split_offset_14_t_uint112(0x08)
                        let expr_908 := _168
                        /// @src 2:7253,7281
                        let _169 := convert_t_uint112_to_t_uint256(expr_908)
                        let expr_909 := fun_mul_2234(expr_907_self, _169)
                        /// @src 2:7245,7281
                        update_storage_value_offset_0t_uint256_to_t_uint256(0x0b, expr_909)
                        let expr_910 := expr_909
                        /// @src 2:7234,7281
                    }
                    /// @src 2:7341,7351
                    let expr_915 := caller()
                    /// @src 2:7353,7360
                    let _170 := var_amount0_758
                    let expr_916 := _170
                    /// @src 2:7362,7369
                    let _171 := var_amount1_760
                    let expr_917 := _171
                    /// @src 2:7371,7373
                    let _172 := var_to_752
                    let expr_918 := _172
                    /// @src 2:7336,7374
                    let _173 := 0xdccd412f0b1252819cb1fd330b93224ca42612892bb3f4f789976e6d81936496
                    {
                        let _174 := allocate_unbounded()
                        let _175 := abi_encode_tuple_t_uint256_t_uint256__to_t_uint256_t_uint256__fromStack(_174 , expr_916, expr_917)
                        log3(_174, sub(_175, _174) , _173, expr_915, expr_918)
                    }
                }

                function fun_encode_2022(var_y_2007) -> var_z_2010 {
                    /// @src 10:282,400
                    /// @src 10:332,341
                    let zero_t_uint224_440 := zero_value_for_split_t_uint224()
                    var_z_2010 := zero_t_uint224_440

                    /// @src 10:365,366
                    let _441 := var_y_2007
                    let expr_2015 := _441
                    /// @src 10:357,367
                    let expr_2016 := convert_t_uint112_to_t_uint224(expr_2015)
                    /// @src 10:370,374
                    let expr_2017 := constant_Q112_2005()
                    /// @src 10:357,374
                    let expr_2018 := checked_mul_t_uint224(expr_2016, expr_2017)

                    /// @src 10:353,374
                    var_z_2010 := expr_2018
                    let expr_2019 := expr_2018

                }

                function fun_getReserves_300() -> var__reserve0_281, var__reserve1_283, var__blockTimestampLast_285 {
                    /// @src 2:1346,1582
                    /// @src 2:1399,1416
                    let zero_t_uint112_6 := zero_value_for_split_t_uint112()
                    var__reserve0_281 := zero_t_uint112_6
                    /// @src 2:1418,1435
                    let zero_t_uint112_7 := zero_value_for_split_t_uint112()
                    var__reserve1_283 := zero_t_uint112_7
                    /// @src 2:1437,1463
                    let zero_t_uint32_8 := zero_value_for_split_t_uint32()
                    var__blockTimestampLast_285 := zero_t_uint32_8

                    /// @src 2:1487,1495
                    let _9 := read_from_storage_split_offset_0_t_uint112(0x08)
                    let expr_288 := _9
                    /// @src 2:1475,1495
                    var__reserve0_281 := expr_288
                    let expr_289 := expr_288
                    /// @src 2:1517,1525
                    let _10 := read_from_storage_split_offset_14_t_uint112(0x08)
                    let expr_292 := _10
                    /// @src 2:1505,1525
                    var__reserve1_283 := expr_292
                    let expr_293 := expr_292
                    /// @src 2:1557,1575
                    let _11 := read_from_storage_split_offset_28_t_uint32(0x08)
                    let expr_296 := _11
                    /// @src 2:1535,1575
                    var__blockTimestampLast_285 := expr_296
                    let expr_297 := expr_296

                }

                function fun_initialize_377(var__token0_354, var__token1_356) {
                    /// @src 2:2420,2635

                    /// @src 2:2510,2520
                    let expr_362 := caller()
                    /// @src 2:2524,2531
                    let _12 := read_from_storage_split_offset_0_t_address(0x05)
                    let expr_363 := _12
                    /// @src 2:2510,2531
                    let expr_364 := eq(cleanup_t_address(expr_362), cleanup_t_address(expr_363))
                    /// @src 2:2502,2556
                    require_helper_t_stringliteral_6e6d2caae3ed190a2586f9b576de92bc80eab72002acec2261bbed89d68a3b37(expr_364)
                    /// @src 2:2595,2602
                    let _13 := var__token0_354
                    let expr_369 := _13
                    /// @src 2:2586,2602
                    update_storage_value_offset_0t_address_to_t_address(0x06, expr_369)
                    let expr_370 := expr_369
                    /// @src 2:2621,2628
                    let _14 := var__token1_356
                    let expr_373 := _14
                    /// @src 2:2612,2628
                    update_storage_value_offset_0t_address_to_t_address(0x07, expr_373)
                    let expr_374 := expr_373

                }

                function fun_min_1943(var_x_1926, var_y_1928) -> var_z_1931 {
                    /// @src 8:97,191
                    /// @src 8:149,155
                    let zero_t_uint256_423 := zero_value_for_split_t_uint256()
                    var_z_1931 := zero_t_uint256_423

                    /// @src 8:171,172
                    let _424 := var_x_1926
                    let expr_1934 := _424
                    /// @src 8:175,176
                    let _425 := var_y_1928
                    let expr_1935 := _425
                    /// @src 8:171,176
                    let expr_1936 := lt(cleanup_t_uint256(expr_1934), cleanup_t_uint256(expr_1935))
                    /// @src 8:171,184
                    let expr_1939
                    switch expr_1936
                    case 0 {
                        /// @src 8:183,184
                        let _426 := var_y_1928
                        let expr_1938 := _426
                        /// @src 8:171,184
                        expr_1939 := expr_1938
                    }
                    default {
                        /// @src 8:179,180
                        let _427 := var_x_1926
                        let expr_1937 := _427
                        /// @src 8:171,184
                        expr_1939 := expr_1937
                    }
                    /// @src 8:167,184
                    var_z_1931 := expr_1939
                    let expr_1940 := expr_1939

                }

                function fun_mint_750(var_to_602) -> var_liquidity_608 {
                    /// @src 2:4591,5819
                    /// @src 2:4649,4663
                    let zero_t_uint256_15 := zero_value_for_split_t_uint256()
                    var_liquidity_608 := zero_t_uint256_15

                    var_liquidity_608 := modifier_lock_606(var_liquidity_608, var_to_602)
                }

                function fun_mint_750_inner(_20, var_to_602) -> var_liquidity_608 {
                    /// @src 2:4591,5819
                    var_liquidity_608 := _20

                    /// @src 2:4717,4728
                    let expr_614_functionIdentifier := 300
                    /// @src 2:4717,4730
                    let expr_615_component_1, expr_615_component_2, expr_615_component_3 := fun_getReserves_300()
                    /// @src 2:4675,4730
                    let var__reserve0_611 := expr_615_component_1
                    let var__reserve1_613 := expr_615_component_2
                    /// @src 2:4778,4784
                    let _21 := read_from_storage_split_offset_0_t_address(0x06)
                    let expr_620 := _21
                    /// @src 2:4771,4785
                    let expr_621_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_620)
                    /// @src 2:4771,4795
                    let expr_622_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_621_address)
                    let expr_622_functionSelector := 0x70a08231
                    /// @src 2:4804,4808
                    let expr_625_address := address()
                    /// @src 2:4796,4809
                    let expr_626 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_625_address)
                    /// @src 2:4771,4810
                    if iszero(extcodesize(expr_622_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _22 := allocate_unbounded()
                    mstore(_22, shift_left_224(expr_622_functionSelector))
                    let _23 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_22, 4) , expr_626)

                    let _24 := staticcall(gas(), expr_622_address,  _22, sub(_23, _22), _22, 32)

                    if iszero(_24) { revert_forward_1() }

                    let expr_627
                    if _24 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_22, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_627 :=  abi_decode_tuple_t_uint256_fromMemory(_22, add(_22, returndatasize()))
                    }
                    /// @src 2:4755,4810
                    let var_balance0_618 := expr_627
                    /// @src 2:4843,4849
                    let _25 := read_from_storage_split_offset_0_t_address(0x07)
                    let expr_632 := _25
                    /// @src 2:4836,4850
                    let expr_633_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_632)
                    /// @src 2:4836,4860
                    let expr_634_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_633_address)
                    let expr_634_functionSelector := 0x70a08231
                    /// @src 2:4869,4873
                    let expr_637_address := address()
                    /// @src 2:4861,4874
                    let expr_638 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_637_address)
                    /// @src 2:4836,4875
                    if iszero(extcodesize(expr_634_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _26 := allocate_unbounded()
                    mstore(_26, shift_left_224(expr_634_functionSelector))
                    let _27 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_26, 4) , expr_638)

                    let _28 := staticcall(gas(), expr_634_address,  _26, sub(_27, _26), _26, 32)

                    if iszero(_28) { revert_forward_1() }

                    let expr_639
                    if _28 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_26, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_639 :=  abi_decode_tuple_t_uint256_fromMemory(_26, add(_26, returndatasize()))
                    }
                    /// @src 2:4820,4875
                    let var_balance1_630 := expr_639
                    /// @src 2:4900,4908
                    let _29 := var_balance0_618
                    let expr_643 := _29
                    /// @src 2:4900,4912
                    let expr_644_self := expr_643
                    let expr_644_functionIdentifier := 2206
                    /// @src 2:4913,4922
                    let _30 := var__reserve0_611
                    let expr_645 := _30
                    /// @src 2:4900,4923
                    let _31 := convert_t_uint112_to_t_uint256(expr_645)
                    let expr_646 := fun_sub_2206(expr_644_self, _31)
                    /// @src 2:4885,4923
                    let var_amount0_642 := expr_646
                    /// @src 2:4948,4956
                    let _32 := var_balance1_630
                    let expr_650 := _32
                    /// @src 2:4948,4960
                    let expr_651_self := expr_650
                    let expr_651_functionIdentifier := 2206
                    /// @src 2:4961,4970
                    let _33 := var__reserve1_613
                    let expr_652 := _33
                    /// @src 2:4948,4971
                    let _34 := convert_t_uint112_to_t_uint256(expr_652)
                    let expr_653 := fun_sub_2206(expr_651_self, _34)
                    /// @src 2:4933,4971
                    let var_amount1_649 := expr_653
                    /// @src 2:4995,5003
                    let expr_657_functionIdentifier := 600
                    /// @src 2:5004,5013
                    let _35 := var__reserve0_611
                    let expr_658 := _35
                    /// @src 2:5015,5024
                    let _36 := var__reserve1_613
                    let expr_659 := _36
                    /// @src 2:4995,5025
                    let expr_660 := fun__mintFee_600(expr_658, expr_659)
                    /// @src 2:4982,5025
                    let var_feeOn_656 := expr_660
                    /// @src 2:5055,5066
                    let _37 := read_from_storage_split_offset_0_t_uint256(0x00)
                    let expr_664 := _37
                    /// @src 2:5035,5066
                    let var__totalSupply_663 := expr_664
                    /// @src 2:5158,5170
                    let _38 := var__totalSupply_663
                    let expr_666 := _38
                    /// @src 2:5174,5175
                    let expr_667 := 0x00
                    /// @src 2:5158,5175
                    let expr_668 := eq(cleanup_t_uint256(expr_666), convert_t_rational_0_by_1_to_t_uint256(expr_667))
                    /// @src 2:5154,5501
                    switch expr_668
                    case 0 {
                        /// @src 2:5404,5408
                        let expr_692_address := linkersymbol("libraries/Math.sol:Math")
                        /// @src 2:5404,5412
                        let expr_693_functionIdentifier := 1943
                        /// @src 2:5413,5420
                        let _39 := var_amount0_642
                        let expr_694 := _39
                        /// @src 2:5413,5424
                        let expr_695_self := expr_694
                        let expr_695_functionIdentifier := 2234
                        /// @src 2:5425,5437
                        let _40 := var__totalSupply_663
                        let expr_696 := _40
                        /// @src 2:5413,5438
                        let expr_697 := fun_mul_2234(expr_695_self, expr_696)
                        /// @src 2:5441,5450
                        let _41 := var__reserve0_611
                        let expr_698 := _41
                        /// @src 2:5413,5450
                        let expr_699 := checked_div_t_uint256(expr_697, convert_t_uint112_to_t_uint256(expr_698))

                        /// @src 2:5452,5459
                        let _42 := var_amount1_649
                        let expr_700 := _42
                        /// @src 2:5452,5463
                        let expr_701_self := expr_700
                        let expr_701_functionIdentifier := 2234
                        /// @src 2:5464,5476
                        let _43 := var__totalSupply_663
                        let expr_702 := _43
                        /// @src 2:5452,5477
                        let expr_703 := fun_mul_2234(expr_701_self, expr_702)
                        /// @src 2:5480,5489
                        let _44 := var__reserve1_613
                        let expr_704 := _44
                        /// @src 2:5452,5489
                        let expr_705 := checked_div_t_uint256(expr_703, convert_t_uint112_to_t_uint256(expr_704))

                        /// @src 2:5404,5490
                        let expr_706 := fun_min_1943(expr_699, expr_705)
                        /// @src 2:5392,5490
                        var_liquidity_608 := expr_706
                        let expr_707 := expr_706
                        /// @src 2:5154,5501
                    }
                    default {
                        /// @src 2:5203,5207
                        let expr_670_address := linkersymbol("libraries/Math.sol:Math")
                        /// @src 2:5203,5212
                        let expr_671_functionIdentifier := 1997
                        /// @src 2:5213,5220
                        let _45 := var_amount0_642
                        let expr_672 := _45
                        /// @src 2:5213,5224
                        let expr_673_self := expr_672
                        let expr_673_functionIdentifier := 2234
                        /// @src 2:5225,5232
                        let _46 := var_amount1_649
                        let expr_674 := _46
                        /// @src 2:5213,5233
                        let expr_675 := fun_mul_2234(expr_673_self, expr_674)
                        /// @src 2:5203,5234
                        let expr_676 := fun_sqrt_1997(expr_675)
                        /// @src 2:5203,5238
                        let expr_677_self := expr_676
                        let expr_677_functionIdentifier := 2206
                        /// @src 2:5239,5256
                        let expr_678 := constant_MINIMUM_LIQUIDITY_220()
                        /// @src 2:5203,5257
                        let expr_679 := fun_sub_2206(expr_677_self, expr_678)
                        /// @src 2:5191,5257
                        var_liquidity_608 := expr_679
                        let expr_680 := expr_679
                        /// @src 2:5270,5275
                        let expr_682_functionIdentifier := 1424
                        /// @src 2:5284,5285
                        let expr_685 := 0x00
                        /// @src 2:5276,5286
                        let expr_686 := convert_t_rational_0_by_1_to_t_address(expr_685)
                        /// @src 2:5288,5305
                        let expr_687 := constant_MINIMUM_LIQUIDITY_220()
                        fun__mint_1424(expr_686, expr_687)
                        /// @src 2:5154,5501
                    }
                    /// @src 2:5518,5527
                    let _47 := var_liquidity_608
                    let expr_712 := _47
                    /// @src 2:5530,5531
                    let expr_713 := 0x00
                    /// @src 2:5518,5531
                    let expr_714 := gt(cleanup_t_uint256(expr_712), convert_t_rational_0_by_1_to_t_uint256(expr_713))
                    /// @src 2:5510,5576
                    require_helper_t_stringliteral_6591c9f5bf1fefaad109b76a20e25c857b696080c952191f86220f001a83663e(expr_714)
                    /// @src 2:5586,5591
                    let expr_718_functionIdentifier := 1424
                    /// @src 2:5592,5594
                    let _48 := var_to_602
                    let expr_719 := _48
                    /// @src 2:5596,5605
                    let _49 := var_liquidity_608
                    let expr_720 := _49
                    fun__mint_1424(expr_719, expr_720)
                    /// @src 2:5617,5624
                    let expr_723_functionIdentifier := 492
                    /// @src 2:5625,5633
                    let _50 := var_balance0_618
                    let expr_724 := _50
                    /// @src 2:5635,5643
                    let _51 := var_balance1_630
                    let expr_725 := _51
                    /// @src 2:5645,5654
                    let _52 := var__reserve0_611
                    let expr_726 := _52
                    /// @src 2:5656,5665
                    let _53 := var__reserve1_613
                    let expr_727 := _53
                    fun__update_492(expr_724, expr_725, expr_726, expr_727)
                    /// @src 2:5680,5685
                    let _54 := var_feeOn_656
                    let expr_730 := _54
                    /// @src 2:5676,5723
                    if expr_730 {
                        /// @src 2:5700,5708
                        let _55 := read_from_storage_split_offset_0_t_uint112(0x08)
                        let expr_734 := _55
                        /// @src 2:5695,5709
                        let expr_735 := convert_t_uint112_to_t_uint256(expr_734)
                        /// @src 2:5695,5713
                        let expr_736_self := expr_735
                        let expr_736_functionIdentifier := 2234
                        /// @src 2:5714,5722
                        let _56 := read_from_storage_split_offset_14_t_uint112(0x08)
                        let expr_737 := _56
                        /// @src 2:5695,5723
                        let _57 := convert_t_uint112_to_t_uint256(expr_737)
                        let expr_738 := fun_mul_2234(expr_736_self, _57)
                        /// @src 2:5687,5723
                        update_storage_value_offset_0t_uint256_to_t_uint256(0x0b, expr_738)
                        let expr_739 := expr_738
                        /// @src 2:5676,5723
                    }
                    /// @src 2:5783,5793
                    let expr_744 := caller()
                    /// @src 2:5795,5802
                    let _58 := var_amount0_642
                    let expr_745 := _58
                    /// @src 2:5804,5811
                    let _59 := var_amount1_649
                    let expr_746 := _59
                    /// @src 2:5778,5812
                    let _60 := 0x4c209b5fc8ad50758f13e2e1088ba56a560dff690a1c6fef26394f4c03821c4f
                    {
                        let _61 := allocate_unbounded()
                        let _62 := abi_encode_tuple_t_uint256_t_uint256__to_t_uint256_t_uint256__fromStack(_61 , expr_745, expr_746)
                        log2(_61, sub(_62, _61) , _60, expr_744)
                    }
                }

                function fun_mul_2234(var_x_2208, var_y_2210) -> var_z_2213 {
                    /// @src 9:419,559
                    /// @src 9:471,477
                    let zero_t_uint256_454 := zero_value_for_split_t_uint256()
                    var_z_2213 := zero_t_uint256_454

                    /// @src 9:497,498
                    let _455 := var_y_2210
                    let expr_2216 := _455
                    /// @src 9:502,503
                    let expr_2217 := 0x00
                    /// @src 9:497,503
                    let expr_2218 := eq(cleanup_t_uint256(expr_2216), convert_t_rational_0_by_1_to_t_uint256(expr_2217))
                    /// @src 9:497,527
                    let expr_2229 := expr_2218
                    if iszero(expr_2229) {
                        /// @src 9:512,513
                        let _456 := var_x_2208
                        let expr_2220 := _456
                        /// @src 9:516,517
                        let _457 := var_y_2210
                        let expr_2221 := _457
                        /// @src 9:512,517
                        let expr_2222 := checked_mul_t_uint256(expr_2220, expr_2221)

                        /// @src 9:508,517
                        var_z_2213 := expr_2222
                        let expr_2223 := expr_2222
                        /// @src 9:507,518
                        let expr_2224 := expr_2223
                        /// @src 9:521,522
                        let _458 := var_y_2210
                        let expr_2225 := _458
                        /// @src 9:507,522
                        let expr_2226 := checked_div_t_uint256(expr_2224, expr_2225)

                        /// @src 9:526,527
                        let _459 := var_x_2208
                        let expr_2227 := _459
                        /// @src 9:507,527
                        let expr_2228 := eq(cleanup_t_uint256(expr_2226), cleanup_t_uint256(expr_2227))
                        /// @src 9:497,527
                        expr_2229 := expr_2228
                    }
                    /// @src 9:489,552
                    require_helper_t_stringliteral_25a0ef6406c6af6852555433653ce478274cd9f03a5dec44d001868a76b3bfdd(expr_2229)

                }

                function fun_permit_1696(var_owner_1618, var_spender_1620, var_value_1622, var_deadline_1624, var_v_1626, var_r_1628, var_s_1630) {
                    /// @src 0:2889,3560

                    /// @src 0:3031,3039
                    let _395 := var_deadline_1624
                    let expr_1635 := _395
                    /// @src 0:3043,3058
                    let expr_1637 := timestamp()
                    /// @src 0:3031,3058
                    let expr_1638 := iszero(lt(cleanup_t_uint256(expr_1635), cleanup_t_uint256(expr_1637)))
                    /// @src 0:3023,3081
                    require_helper_t_stringliteral_47797eaaf6df6dc2efdb1e824209400a8293aff4c1e7f6d90fcc4b3e3ba18a87(expr_1638)
                    /// @src 0:3193,3209
                    let _396 := read_from_storage_split_offset_0_t_bytes32(0x03)
                    let expr_1648 := _396
                    /// @src 0:3248,3263
                    let expr_1652 := constant_PERMIT_TYPEHASH_1348()
                    /// @src 0:3265,3270
                    let _397 := var_owner_1618
                    let expr_1653 := _397
                    /// @src 0:3272,3279
                    let _398 := var_spender_1620
                    let expr_1654 := _398
                    /// @src 0:3281,3286
                    let _399 := var_value_1622
                    let expr_1655 := _399
                    /// @src 0:3288,3294
                    let _400 := 0x04
                    let expr_1656 := _400
                    /// @src 0:3295,3300
                    let _401 := var_owner_1618
                    let expr_1657 := _401
                    /// @src 0:3288,3301
                    let _402 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1656,expr_1657)
                    /// @src 0:3288,3303
                    let _404 := read_from_storage_split_offset_0_t_uint256(_402)
                    let _403 := increment_t_uint256(_404)
                    update_storage_value_offset_0t_uint256_to_t_uint256(_402, _403)
                    let expr_1659 := _404
                    /// @src 0:3305,3313
                    let _405 := var_deadline_1624
                    let expr_1660 := _405
                    /// @src 0:3237,3314

                    let expr_1661_mpos := allocate_unbounded()
                    let _406 := add(expr_1661_mpos, 0x20)

                    let _407 := abi_encode_tuple_t_bytes32_t_address_t_address_t_uint256_t_uint256_t_uint256__to_t_bytes32_t_address_t_address_t_uint256_t_uint256_t_uint256__fromStack(_406, expr_1652, expr_1653, expr_1654, expr_1655, expr_1659, expr_1660)
                    mstore(expr_1661_mpos, sub(_407, add(expr_1661_mpos, 0x20)))
                    finalize_allocation(expr_1661_mpos, sub(_407, expr_1661_mpos))
                    /// @src 0:3227,3315
                    let expr_1662 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_1661_mpos), array_length_t_bytes_memory_ptr(expr_1661_mpos))
                    /// @src 0:3131,3329

                    let expr_1663_mpos := allocate_unbounded()
                    let _408 := add(expr_1663_mpos, 0x20)

                    let _409 := abi_encode_tuple_packed_t_stringliteral_301a50b291d33ce1e8e9064e3f6a6c51d902ec22892b50d58abf6357c6a45541_t_bytes32_t_bytes32__to_t_string_memory_ptr_t_bytes32_t_bytes32__nonPadded_inplace_fromStack(_408, expr_1648, expr_1662)
                    mstore(expr_1663_mpos, sub(_409, add(expr_1663_mpos, 0x20)))
                    finalize_allocation(expr_1663_mpos, sub(_409, expr_1663_mpos))
                    /// @src 0:3108,3339
                    let expr_1664 := keccak256(array_dataslot_t_bytes_memory_ptr(expr_1663_mpos), array_length_t_bytes_memory_ptr(expr_1663_mpos))
                    /// @src 0:3091,3339
                    let var_digest_1643 := expr_1664
                    /// @src 0:3386,3392
                    let _410 := var_digest_1643
                    let expr_1669 := _410
                    /// @src 0:3394,3395
                    let _411 := var_v_1626
                    let expr_1670 := _411
                    /// @src 0:3397,3398
                    let _412 := var_r_1628
                    let expr_1671 := _412
                    /// @src 0:3400,3401
                    let _413 := var_s_1630
                    let expr_1672 := _413
                    /// @src 0:3376,3402

                    let _414 := allocate_unbounded()
                    let _415 := abi_encode_tuple_t_bytes32_t_uint8_t_bytes32_t_bytes32__to_t_bytes32_t_uint8_t_bytes32_t_bytes32__fromStack(_414 , expr_1669, expr_1670, expr_1671, expr_1672)

                    mstore(0, 0)

                    let _416 := staticcall(gas(), 1 , _414, sub(_415, _414), 0, 32)
                    if iszero(_416) { revert_forward_1() }
                    let expr_1673 := shift_left_0(mload(0))
                    /// @src 0:3349,3402
                    let var_recoveredAddress_1667 := expr_1673
                    /// @src 0:3420,3436
                    let _417 := var_recoveredAddress_1667
                    let expr_1676 := _417
                    /// @src 0:3448,3449
                    let expr_1679 := 0x00
                    /// @src 0:3440,3450
                    let expr_1680 := convert_t_rational_0_by_1_to_t_address(expr_1679)
                    /// @src 0:3420,3450
                    let expr_1681 := iszero(eq(cleanup_t_address(expr_1676), cleanup_t_address(expr_1680)))
                    /// @src 0:3420,3479
                    let expr_1685 := expr_1681
                    if expr_1685 {
                        /// @src 0:3454,3470
                        let _418 := var_recoveredAddress_1667
                        let expr_1682 := _418
                        /// @src 0:3474,3479
                        let _419 := var_owner_1618
                        let expr_1683 := _419
                        /// @src 0:3454,3479
                        let expr_1684 := eq(cleanup_t_address(expr_1682), cleanup_t_address(expr_1683))
                        /// @src 0:3420,3479
                        expr_1685 := expr_1684
                    }
                    /// @src 0:3412,3512
                    require_helper_t_stringliteral_2d893fc9f5fa2494c39ecec82df2778b33226458ccce3b9a56f5372437d54a56(expr_1685)
                    /// @src 0:3522,3530
                    let expr_1689_functionIdentifier := 1483
                    /// @src 0:3531,3536
                    let _420 := var_owner_1618
                    let expr_1690 := _420
                    /// @src 0:3538,3545
                    let _421 := var_spender_1620
                    let expr_1691 := _421
                    /// @src 0:3547,3552
                    let _422 := var_value_1622
                    let expr_1692 := _422
                    fun__approve_1483(expr_1690, expr_1691, expr_1692)

                }

                function fun_skim_1211(var_to_1162) {
                    /// @src 2:9406,9744

                    modifier_lock_1166(var_to_1162)
                }

                function fun_skim_1211_inner(var_to_1162) {
                    /// @src 2:9406,9744

                    /// @src 2:9483,9489
                    let _271 := read_from_storage_split_offset_0_t_address(0x06)
                    let expr_1170 := _271
                    /// @src 2:9465,9489
                    let var__token0_1169 := expr_1170
                    /// @src 2:9532,9538
                    let _272 := read_from_storage_split_offset_0_t_address(0x07)
                    let expr_1174 := _272
                    /// @src 2:9514,9538
                    let var__token1_1173 := expr_1174
                    /// @src 2:9563,9576
                    let expr_1176_functionIdentifier := 343
                    /// @src 2:9577,9584
                    let _273 := var__token0_1169
                    let expr_1177 := _273
                    /// @src 2:9586,9588
                    let _274 := var_to_1162
                    let expr_1178 := _274
                    /// @src 2:9597,9604
                    let _275 := var__token0_1169
                    let expr_1180 := _275
                    /// @src 2:9590,9605
                    let expr_1181_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_1180)
                    /// @src 2:9590,9615
                    let expr_1182_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_1181_address)
                    let expr_1182_functionSelector := 0x70a08231
                    /// @src 2:9624,9628
                    let expr_1185_address := address()
                    /// @src 2:9616,9629
                    let expr_1186 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_1185_address)
                    /// @src 2:9590,9630
                    if iszero(extcodesize(expr_1182_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _276 := allocate_unbounded()
                    mstore(_276, shift_left_224(expr_1182_functionSelector))
                    let _277 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_276, 4) , expr_1186)

                    let _278 := staticcall(gas(), expr_1182_address,  _276, sub(_277, _276), _276, 32)

                    if iszero(_278) { revert_forward_1() }

                    let expr_1187
                    if _278 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_276, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_1187 :=  abi_decode_tuple_t_uint256_fromMemory(_276, add(_276, returndatasize()))
                    }
                    /// @src 2:9590,9634
                    let expr_1188_self := expr_1187
                    let expr_1188_functionIdentifier := 2206
                    /// @src 2:9635,9643
                    let _279 := read_from_storage_split_offset_0_t_uint112(0x08)
                    let expr_1189 := _279
                    /// @src 2:9590,9644
                    let _280 := convert_t_uint112_to_t_uint256(expr_1189)
                    let expr_1190 := fun_sub_2206(expr_1188_self, _280)
                    fun__safeTransfer_343(expr_1177, expr_1178, expr_1190)
                    /// @src 2:9655,9668
                    let expr_1193_functionIdentifier := 343
                    /// @src 2:9669,9676
                    let _281 := var__token1_1173
                    let expr_1194 := _281
                    /// @src 2:9678,9680
                    let _282 := var_to_1162
                    let expr_1195 := _282
                    /// @src 2:9689,9696
                    let _283 := var__token1_1173
                    let expr_1197 := _283
                    /// @src 2:9682,9697
                    let expr_1198_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_1197)
                    /// @src 2:9682,9707
                    let expr_1199_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_1198_address)
                    let expr_1199_functionSelector := 0x70a08231
                    /// @src 2:9716,9720
                    let expr_1202_address := address()
                    /// @src 2:9708,9721
                    let expr_1203 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_1202_address)
                    /// @src 2:9682,9722
                    if iszero(extcodesize(expr_1199_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _284 := allocate_unbounded()
                    mstore(_284, shift_left_224(expr_1199_functionSelector))
                    let _285 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_284, 4) , expr_1203)

                    let _286 := staticcall(gas(), expr_1199_address,  _284, sub(_285, _284), _284, 32)

                    if iszero(_286) { revert_forward_1() }

                    let expr_1204
                    if _286 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_284, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_1204 :=  abi_decode_tuple_t_uint256_fromMemory(_284, add(_284, returndatasize()))
                    }
                    /// @src 2:9682,9726
                    let expr_1205_self := expr_1204
                    let expr_1205_functionIdentifier := 2206
                    /// @src 2:9727,9735
                    let _287 := read_from_storage_split_offset_14_t_uint112(0x08)
                    let expr_1206 := _287
                    /// @src 2:9682,9736
                    let _288 := convert_t_uint112_to_t_uint256(expr_1206)
                    let expr_1207 := fun_sub_2206(expr_1205_self, _288)
                    fun__safeTransfer_343(expr_1194, expr_1195, expr_1207)

                }

                function fun_sqrt_1997(var_y_1945) -> var_z_1948 {
                    /// @src 8:306,598
                    /// @src 8:351,357
                    let zero_t_uint256_428 := zero_value_for_split_t_uint256()
                    var_z_1948 := zero_t_uint256_428

                    /// @src 8:373,374
                    let _429 := var_y_1945
                    let expr_1950 := _429
                    /// @src 8:377,378
                    let expr_1951 := 0x03
                    /// @src 8:373,378
                    let expr_1952 := gt(cleanup_t_uint256(expr_1950), convert_t_rational_3_by_1_to_t_uint256(expr_1951))
                    /// @src 8:369,592
                    switch expr_1952
                    case 0 {
                        /// @src 8:554,555
                        let _430 := var_y_1945
                        let expr_1986 := _430
                        /// @src 8:559,560
                        let expr_1987 := 0x00
                        /// @src 8:554,560
                        let expr_1988 := iszero(eq(cleanup_t_uint256(expr_1986), convert_t_rational_0_by_1_to_t_uint256(expr_1987)))
                        /// @src 8:550,592
                        if expr_1988 {
                            /// @src 8:580,581
                            let expr_1990 := 0x01
                            /// @src 8:576,581
                            let _431 := convert_t_rational_1_by_1_to_t_uint256(expr_1990)
                            var_z_1948 := _431
                            let expr_1991 := _431
                            /// @src 8:550,592
                        }
                        /// @src 8:369,592
                    }
                    default {
                        /// @src 8:398,399
                        let _432 := var_y_1945
                        let expr_1954 := _432
                        /// @src 8:394,399
                        var_z_1948 := expr_1954
                        let expr_1955 := expr_1954
                        /// @src 8:422,423
                        let _433 := var_y_1945
                        let expr_1959 := _433
                        /// @src 8:426,427
                        let expr_1960 := 0x02
                        /// @src 8:422,427
                        let expr_1961 := checked_div_t_uint256(expr_1959, convert_t_rational_2_by_1_to_t_uint256(expr_1960))

                        /// @src 8:430,431
                        let expr_1962 := 0x01
                        /// @src 8:422,431
                        let expr_1963 := checked_add_t_uint256(expr_1961, convert_t_rational_1_by_1_to_t_uint256(expr_1962))

                        /// @src 8:413,431
                        let var_x_1958 := expr_1963
                        /// @src 8:445,534
                        for {
                            } 1 {
                        }
                        {
                            /// @src 8:452,453
                            let _434 := var_x_1958
                            let expr_1965 := _434
                            /// @src 8:456,457
                            let _435 := var_z_1948
                            let expr_1966 := _435
                            /// @src 8:452,457
                            let expr_1967 := lt(cleanup_t_uint256(expr_1965), cleanup_t_uint256(expr_1966))
                            if iszero(expr_1967) { break }
                            /// @src 8:481,482
                            let _436 := var_x_1958
                            let expr_1969 := _436
                            /// @src 8:477,482
                            var_z_1948 := expr_1969
                            let expr_1970 := expr_1969
                            /// @src 8:505,506
                            let _437 := var_y_1945
                            let expr_1973 := _437
                            /// @src 8:509,510
                            let _438 := var_x_1958
                            let expr_1974 := _438
                            /// @src 8:505,510
                            let expr_1975 := checked_div_t_uint256(expr_1973, expr_1974)

                            /// @src 8:513,514
                            let _439 := var_x_1958
                            let expr_1976 := _439
                            /// @src 8:505,514
                            let expr_1977 := checked_add_t_uint256(expr_1975, expr_1976)

                            /// @src 8:504,515
                            let expr_1978 := expr_1977
                            /// @src 8:518,519
                            let expr_1979 := 0x02
                            /// @src 8:504,519
                            let expr_1980 := checked_div_t_uint256(expr_1978, convert_t_rational_2_by_1_to_t_uint256(expr_1979))

                            /// @src 8:500,519
                            var_x_1958 := expr_1980
                            let expr_1981 := expr_1980
                        }
                        /// @src 8:369,592
                    }

                }

                function fun_sub_2206(var_x_2186, var_y_2188) -> var_z_2191 {
                    /// @src 9:286,413
                    /// @src 9:338,344
                    let zero_t_uint256_450 := zero_value_for_split_t_uint256()
                    var_z_2191 := zero_t_uint256_450

                    /// @src 9:369,370
                    let _451 := var_x_2186
                    let expr_2195 := _451
                    /// @src 9:373,374
                    let _452 := var_y_2188
                    let expr_2196 := _452
                    /// @src 9:369,374
                    let expr_2197 := checked_sub_t_uint256(expr_2195, expr_2196)

                    /// @src 9:365,374
                    var_z_2191 := expr_2197
                    let expr_2198 := expr_2197
                    /// @src 9:364,375
                    let expr_2199 := expr_2198
                    /// @src 9:379,380
                    let _453 := var_x_2186
                    let expr_2200 := _453
                    /// @src 9:364,380
                    let expr_2201 := iszero(gt(cleanup_t_uint256(expr_2199), cleanup_t_uint256(expr_2200)))
                    /// @src 9:356,406
                    require_helper_t_stringliteral_03b20b9f6e6e7905f077509fd420fb44afc685f254bcefe49147296e1ba25590(expr_2201)

                }

                function fun_swap_1160(var_amount0Out_924, var_amount1Out_926, var_to_928, var_data_930_offset, var_data_930_length) {
                    /// @src 2:7490,9360

                    modifier_lock_934(var_amount0Out_924, var_amount1Out_926, var_to_928, var_data_930_offset, var_data_930_length)
                }

                function fun_swap_1160_inner(var_amount0Out_924, var_amount1Out_926, var_to_928, var_data_930_offset, var_data_930_length) {
                    /// @src 2:7490,9360

                    /// @src 2:7612,7622
                    let _190 := var_amount0Out_924
                    let expr_937 := _190
                    /// @src 2:7625,7626
                    let expr_938 := 0x00
                    /// @src 2:7612,7626
                    let expr_939 := gt(cleanup_t_uint256(expr_937), convert_t_rational_0_by_1_to_t_uint256(expr_938))
                    /// @src 2:7612,7644
                    let expr_943 := expr_939
                    if iszero(expr_943) {
                        /// @src 2:7630,7640
                        let _191 := var_amount1Out_926
                        let expr_940 := _191
                        /// @src 2:7643,7644
                        let expr_941 := 0x00
                        /// @src 2:7630,7644
                        let expr_942 := gt(cleanup_t_uint256(expr_940), convert_t_rational_0_by_1_to_t_uint256(expr_941))
                        /// @src 2:7612,7644
                        expr_943 := expr_942
                    }
                    /// @src 2:7604,7686
                    require_helper_t_stringliteral_05339493da7e2cbe77e17beadf6b91132eb307939495f5f1797bf88d95539e83(expr_943)
                    /// @src 2:7738,7749
                    let expr_951_functionIdentifier := 300
                    /// @src 2:7738,7751
                    let expr_952_component_1, expr_952_component_2, expr_952_component_3 := fun_getReserves_300()
                    /// @src 2:7696,7751
                    let var__reserve0_948 := expr_952_component_1
                    let var__reserve1_950 := expr_952_component_2
                    /// @src 2:7784,7794
                    let _192 := var_amount0Out_924
                    let expr_955 := _192
                    /// @src 2:7797,7806
                    let _193 := var__reserve0_948
                    let expr_956 := _193
                    /// @src 2:7784,7806
                    let expr_957 := lt(cleanup_t_uint256(expr_955), convert_t_uint112_to_t_uint256(expr_956))
                    /// @src 2:7784,7832
                    let expr_961 := expr_957
                    if expr_961 {
                        /// @src 2:7810,7820
                        let _194 := var_amount1Out_926
                        let expr_958 := _194
                        /// @src 2:7823,7832
                        let _195 := var__reserve1_950
                        let expr_959 := _195
                        /// @src 2:7810,7832
                        let expr_960 := lt(cleanup_t_uint256(expr_958), convert_t_uint112_to_t_uint256(expr_959))
                        /// @src 2:7784,7832
                        expr_961 := expr_960
                    }
                    /// @src 2:7776,7870
                    require_helper_t_stringliteral_3f354ef449b2a9b081220ce21f57691008110b653edc191d8288e60cef58bb5f(expr_961)
                    /// @src 2:7881,7894
                    let var_balance0_966
                    let zero_t_uint256_196 := zero_value_for_split_t_uint256()
                    var_balance0_966 := zero_t_uint256_196
                    /// @src 2:7904,7917
                    let var_balance1_969
                    let zero_t_uint256_197 := zero_value_for_split_t_uint256()
                    var_balance1_969 := zero_t_uint256_197
                    /// @src 2:8010,8016
                    let _198 := read_from_storage_split_offset_0_t_address(0x06)
                    let expr_973 := _198
                    /// @src 2:7992,8016
                    let var__token0_972 := expr_973
                    /// @src 2:8044,8050
                    let _199 := read_from_storage_split_offset_0_t_address(0x07)
                    let expr_977 := _199
                    /// @src 2:8026,8050
                    let var__token1_976 := expr_977
                    /// @src 2:8068,8070
                    let _200 := var_to_928
                    let expr_980 := _200
                    /// @src 2:8074,8081
                    let _201 := var__token0_972
                    let expr_981 := _201
                    /// @src 2:8068,8081
                    let expr_982 := iszero(eq(cleanup_t_address(expr_980), cleanup_t_address(expr_981)))
                    /// @src 2:8068,8098
                    let expr_986 := expr_982
                    if expr_986 {
                        /// @src 2:8085,8087
                        let _202 := var_to_928
                        let expr_983 := _202
                        /// @src 2:8091,8098
                        let _203 := var__token1_976
                        let expr_984 := _203
                        /// @src 2:8085,8098
                        let expr_985 := iszero(eq(cleanup_t_address(expr_983), cleanup_t_address(expr_984)))
                        /// @src 2:8068,8098
                        expr_986 := expr_985
                    }
                    /// @src 2:8060,8124
                    require_helper_t_stringliteral_25d395026e6e4dd4e9808c7d6d3dd1f45abaf4874ae71f7161fff58de03154d3(expr_986)
                    /// @src 2:8138,8148
                    let _204 := var_amount0Out_924
                    let expr_990 := _204
                    /// @src 2:8151,8152
                    let expr_991 := 0x00
                    /// @src 2:8138,8152
                    let expr_992 := gt(cleanup_t_uint256(expr_990), convert_t_rational_0_by_1_to_t_uint256(expr_991))
                    /// @src 2:8134,8192
                    if expr_992 {
                        /// @src 2:8154,8167
                        let expr_993_functionIdentifier := 343
                        /// @src 2:8168,8175
                        let _205 := var__token0_972
                        let expr_994 := _205
                        /// @src 2:8177,8179
                        let _206 := var_to_928
                        let expr_995 := _206
                        /// @src 2:8181,8191
                        let _207 := var_amount0Out_924
                        let expr_996 := _207
                        fun__safeTransfer_343(expr_994, expr_995, expr_996)
                        /// @src 2:8134,8192
                    }
                    /// @src 2:8240,8250
                    let _208 := var_amount1Out_926
                    let expr_1000 := _208
                    /// @src 2:8253,8254
                    let expr_1001 := 0x00
                    /// @src 2:8240,8254
                    let expr_1002 := gt(cleanup_t_uint256(expr_1000), convert_t_rational_0_by_1_to_t_uint256(expr_1001))
                    /// @src 2:8236,8294
                    if expr_1002 {
                        /// @src 2:8256,8269
                        let expr_1003_functionIdentifier := 343
                        /// @src 2:8270,8277
                        let _209 := var__token1_976
                        let expr_1004 := _209
                        /// @src 2:8279,8281
                        let _210 := var_to_928
                        let expr_1005 := _210
                        /// @src 2:8283,8293
                        let _211 := var_amount1Out_926
                        let expr_1006 := _211
                        fun__safeTransfer_343(expr_1004, expr_1005, expr_1006)
                        /// @src 2:8236,8294
                    }
                    /// @src 2:8342,8346
                    let _212_offset := var_data_930_offset
                    let _212_length := var_data_930_length
                    let expr_1010_offset := _212_offset
                    let expr_1010_length := _212_length
                    /// @src 2:8342,8353
                    let expr_1011 := array_length_t_bytes_calldata_ptr(expr_1010_offset, expr_1010_length)
                    /// @src 2:8356,8357
                    let expr_1012 := 0x00
                    /// @src 2:8342,8357
                    let expr_1013 := gt(cleanup_t_uint256(expr_1011), convert_t_rational_0_by_1_to_t_uint256(expr_1012))
                    /// @src 2:8338,8435
                    if expr_1013 {
                        /// @src 2:8376,8378
                        let _213 := var_to_928
                        let expr_1015 := _213
                        /// @src 2:8359,8379
                        let expr_1016_address := convert_t_address_to_t_contract$_IUniswapV2Callee_$1795(expr_1015)
                        /// @src 2:8359,8393
                        let expr_1017_address := convert_t_contract$_IUniswapV2Callee_$1795_to_t_address(expr_1016_address)
                        let expr_1017_functionSelector := 0x10d1e85c
                        /// @src 2:8394,8404
                        let expr_1019 := caller()
                        /// @src 2:8406,8416
                        let _214 := var_amount0Out_924
                        let expr_1020 := _214
                        /// @src 2:8418,8428
                        let _215 := var_amount1Out_926
                        let expr_1021 := _215
                        /// @src 2:8430,8434
                        let _216_offset := var_data_930_offset
                        let _216_length := var_data_930_length
                        let expr_1022_offset := _216_offset
                        let expr_1022_length := _216_length
                        /// @src 2:8359,8435
                        if iszero(extcodesize(expr_1017_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                        // storage for arguments and returned data
                        let _217 := allocate_unbounded()
                        mstore(_217, shift_left_224(expr_1017_functionSelector))
                        let _218 := abi_encode_tuple_t_address_t_uint256_t_uint256_t_bytes_calldata_ptr__to_t_address_t_uint256_t_uint256_t_bytes_memory_ptr__fromStack(add(_217, 4) , expr_1019, expr_1020, expr_1021, expr_1022_offset, expr_1022_length)

                        let _219 := call(gas(), expr_1017_address,  0,  _217, sub(_218, _217), _217, 0)

                        if iszero(_219) { revert_forward_1() }

                        if _219 {

                            // update freeMemoryPointer according to dynamic return size
                            finalize_allocation(_217, returndatasize())

                            // decode return parameters from external try-call into retVars
                            abi_decode_tuple__fromMemory(_217, add(_217, returndatasize()))
                        }
                        /// @src 2:8338,8435
                    }
                    /// @src 2:8463,8470
                    let _220 := var__token0_972
                    let expr_1028 := _220
                    /// @src 2:8456,8471
                    let expr_1029_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_1028)
                    /// @src 2:8456,8481
                    let expr_1030_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_1029_address)
                    let expr_1030_functionSelector := 0x70a08231
                    /// @src 2:8490,8494
                    let expr_1033_address := address()
                    /// @src 2:8482,8495
                    let expr_1034 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_1033_address)
                    /// @src 2:8456,8496
                    if iszero(extcodesize(expr_1030_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _221 := allocate_unbounded()
                    mstore(_221, shift_left_224(expr_1030_functionSelector))
                    let _222 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_221, 4) , expr_1034)

                    let _223 := staticcall(gas(), expr_1030_address,  _221, sub(_222, _221), _221, 32)

                    if iszero(_223) { revert_forward_1() }

                    let expr_1035
                    if _223 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_221, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_1035 :=  abi_decode_tuple_t_uint256_fromMemory(_221, add(_221, returndatasize()))
                    }
                    /// @src 2:8445,8496
                    var_balance0_966 := expr_1035
                    let expr_1036 := expr_1035
                    /// @src 2:8524,8531
                    let _224 := var__token1_976
                    let expr_1040 := _224
                    /// @src 2:8517,8532
                    let expr_1041_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_1040)
                    /// @src 2:8517,8542
                    let expr_1042_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_1041_address)
                    let expr_1042_functionSelector := 0x70a08231
                    /// @src 2:8551,8555
                    let expr_1045_address := address()
                    /// @src 2:8543,8556
                    let expr_1046 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_1045_address)
                    /// @src 2:8517,8557
                    if iszero(extcodesize(expr_1042_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _225 := allocate_unbounded()
                    mstore(_225, shift_left_224(expr_1042_functionSelector))
                    let _226 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_225, 4) , expr_1046)

                    let _227 := staticcall(gas(), expr_1042_address,  _225, sub(_226, _225), _225, 32)

                    if iszero(_227) { revert_forward_1() }

                    let expr_1047
                    if _227 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_225, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_1047 :=  abi_decode_tuple_t_uint256_fromMemory(_225, add(_225, returndatasize()))
                    }
                    /// @src 2:8506,8557
                    var_balance1_969 := expr_1047
                    let expr_1048 := expr_1047
                    /// @src 2:8594,8602
                    let _228 := var_balance0_966
                    let expr_1053 := _228
                    /// @src 2:8605,8614
                    let _229 := var__reserve0_948
                    let expr_1054 := _229
                    /// @src 2:8617,8627
                    let _230 := var_amount0Out_924
                    let expr_1055 := _230
                    /// @src 2:8605,8627
                    let expr_1056 := checked_sub_t_uint256(convert_t_uint112_to_t_uint256(expr_1054), expr_1055)

                    /// @src 2:8594,8627
                    let expr_1057 := gt(cleanup_t_uint256(expr_1053), cleanup_t_uint256(expr_1056))
                    /// @src 2:8594,8669
                    let expr_1065
                    switch expr_1057
                    case 0 {
                        /// @src 2:8668,8669
                        let expr_1064 := 0x00
                        /// @src 2:8594,8669
                        expr_1065 := convert_t_rational_0_by_1_to_t_uint256(expr_1064)
                    }
                    default {
                        /// @src 2:8630,8638
                        let _231 := var_balance0_966
                        let expr_1058 := _231
                        /// @src 2:8642,8651
                        let _232 := var__reserve0_948
                        let expr_1059 := _232
                        /// @src 2:8654,8664
                        let _233 := var_amount0Out_924
                        let expr_1060 := _233
                        /// @src 2:8642,8664
                        let expr_1061 := checked_sub_t_uint256(convert_t_uint112_to_t_uint256(expr_1059), expr_1060)

                        /// @src 2:8641,8665
                        let expr_1062 := expr_1061
                        /// @src 2:8630,8665
                        let expr_1063 := checked_sub_t_uint256(expr_1058, expr_1062)

                        /// @src 2:8594,8669
                        expr_1065 := expr_1063
                    }
                    /// @src 2:8577,8669
                    let var_amount0In_1052 := expr_1065
                    /// @src 2:8696,8704
                    let _234 := var_balance1_969
                    let expr_1069 := _234
                    /// @src 2:8707,8716
                    let _235 := var__reserve1_950
                    let expr_1070 := _235
                    /// @src 2:8719,8729
                    let _236 := var_amount1Out_926
                    let expr_1071 := _236
                    /// @src 2:8707,8729
                    let expr_1072 := checked_sub_t_uint256(convert_t_uint112_to_t_uint256(expr_1070), expr_1071)

                    /// @src 2:8696,8729
                    let expr_1073 := gt(cleanup_t_uint256(expr_1069), cleanup_t_uint256(expr_1072))
                    /// @src 2:8696,8771
                    let expr_1081
                    switch expr_1073
                    case 0 {
                        /// @src 2:8770,8771
                        let expr_1080 := 0x00
                        /// @src 2:8696,8771
                        expr_1081 := convert_t_rational_0_by_1_to_t_uint256(expr_1080)
                    }
                    default {
                        /// @src 2:8732,8740
                        let _237 := var_balance1_969
                        let expr_1074 := _237
                        /// @src 2:8744,8753
                        let _238 := var__reserve1_950
                        let expr_1075 := _238
                        /// @src 2:8756,8766
                        let _239 := var_amount1Out_926
                        let expr_1076 := _239
                        /// @src 2:8744,8766
                        let expr_1077 := checked_sub_t_uint256(convert_t_uint112_to_t_uint256(expr_1075), expr_1076)

                        /// @src 2:8743,8767
                        let expr_1078 := expr_1077
                        /// @src 2:8732,8767
                        let expr_1079 := checked_sub_t_uint256(expr_1074, expr_1078)

                        /// @src 2:8696,8771
                        expr_1081 := expr_1079
                    }
                    /// @src 2:8679,8771
                    let var_amount1In_1068 := expr_1081
                    /// @src 2:8789,8798
                    let _240 := var_amount0In_1052
                    let expr_1084 := _240
                    /// @src 2:8801,8802
                    let expr_1085 := 0x00
                    /// @src 2:8789,8802
                    let expr_1086 := gt(cleanup_t_uint256(expr_1084), convert_t_rational_0_by_1_to_t_uint256(expr_1085))
                    /// @src 2:8789,8819
                    let expr_1090 := expr_1086
                    if iszero(expr_1090) {
                        /// @src 2:8806,8815
                        let _241 := var_amount1In_1068
                        let expr_1087 := _241
                        /// @src 2:8818,8819
                        let expr_1088 := 0x00
                        /// @src 2:8806,8819
                        let expr_1089 := gt(cleanup_t_uint256(expr_1087), convert_t_rational_0_by_1_to_t_uint256(expr_1088))
                        /// @src 2:8789,8819
                        expr_1090 := expr_1089
                    }
                    /// @src 2:8781,8860
                    require_helper_t_stringliteral_10e2efc32d8a31d3b2c11a545b3ed09c2dbabc58ef6de4033929d0002e425b67(expr_1090)
                    /// @src 2:8968,8976
                    let _242 := var_balance0_966
                    let expr_1096 := _242
                    /// @src 2:8968,8980
                    let expr_1097_self := expr_1096
                    let expr_1097_functionIdentifier := 2234
                    /// @src 2:8981,8985
                    let expr_1098 := 0x03e8
                    /// @src 2:8968,8986
                    let _243 := convert_t_rational_1000_by_1_to_t_uint256(expr_1098)
                    let expr_1099 := fun_mul_2234(expr_1097_self, _243)
                    /// @src 2:8968,8990
                    let expr_1100_self := expr_1099
                    let expr_1100_functionIdentifier := 2206
                    /// @src 2:8991,9000
                    let _244 := var_amount0In_1052
                    let expr_1101 := _244
                    /// @src 2:8991,9004
                    let expr_1102_self := expr_1101
                    let expr_1102_functionIdentifier := 2234
                    /// @src 2:9005,9006
                    let expr_1103 := 0x03
                    /// @src 2:8991,9007
                    let _245 := convert_t_rational_3_by_1_to_t_uint256(expr_1103)
                    let expr_1104 := fun_mul_2234(expr_1102_self, _245)
                    /// @src 2:8968,9008
                    let expr_1105 := fun_sub_2206(expr_1100_self, expr_1104)
                    /// @src 2:8944,9008
                    let var_balance0Adjusted_1095 := expr_1105
                    /// @src 2:9042,9050
                    let _246 := var_balance1_969
                    let expr_1109 := _246
                    /// @src 2:9042,9054
                    let expr_1110_self := expr_1109
                    let expr_1110_functionIdentifier := 2234
                    /// @src 2:9055,9059
                    let expr_1111 := 0x03e8
                    /// @src 2:9042,9060
                    let _247 := convert_t_rational_1000_by_1_to_t_uint256(expr_1111)
                    let expr_1112 := fun_mul_2234(expr_1110_self, _247)
                    /// @src 2:9042,9064
                    let expr_1113_self := expr_1112
                    let expr_1113_functionIdentifier := 2206
                    /// @src 2:9065,9074
                    let _248 := var_amount1In_1068
                    let expr_1114 := _248
                    /// @src 2:9065,9078
                    let expr_1115_self := expr_1114
                    let expr_1115_functionIdentifier := 2234
                    /// @src 2:9079,9080
                    let expr_1116 := 0x03
                    /// @src 2:9065,9081
                    let _249 := convert_t_rational_3_by_1_to_t_uint256(expr_1116)
                    let expr_1117 := fun_mul_2234(expr_1115_self, _249)
                    /// @src 2:9042,9082
                    let expr_1118 := fun_sub_2206(expr_1113_self, expr_1117)
                    /// @src 2:9018,9082
                    let var_balance1Adjusted_1108 := expr_1118
                    /// @src 2:9100,9116
                    let _250 := var_balance0Adjusted_1095
                    let expr_1121 := _250
                    /// @src 2:9100,9120
                    let expr_1122_self := expr_1121
                    let expr_1122_functionIdentifier := 2234
                    /// @src 2:9121,9137
                    let _251 := var_balance1Adjusted_1108
                    let expr_1123 := _251
                    /// @src 2:9100,9138
                    let expr_1124 := fun_mul_2234(expr_1122_self, expr_1123)
                    /// @src 2:9147,9156
                    let _252 := var__reserve0_948
                    let expr_1127 := _252
                    /// @src 2:9142,9157
                    let expr_1128 := convert_t_uint112_to_t_uint256(expr_1127)
                    /// @src 2:9142,9161
                    let expr_1129_self := expr_1128
                    let expr_1129_functionIdentifier := 2234
                    /// @src 2:9162,9171
                    let _253 := var__reserve1_950
                    let expr_1130 := _253
                    /// @src 2:9142,9172
                    let _254 := convert_t_uint112_to_t_uint256(expr_1130)
                    let expr_1131 := fun_mul_2234(expr_1129_self, _254)
                    /// @src 2:9142,9176
                    let expr_1132_self := expr_1131
                    let expr_1132_functionIdentifier := 2234
                    /// @src 2:9177,9184
                    let expr_1135 := 0x0f4240
                    /// @src 2:9142,9185
                    let _255 := convert_t_rational_1000000_by_1_to_t_uint256(expr_1135)
                    let expr_1136 := fun_mul_2234(expr_1132_self, _255)
                    /// @src 2:9100,9185
                    let expr_1137 := iszero(lt(cleanup_t_uint256(expr_1124), cleanup_t_uint256(expr_1136)))
                    /// @src 2:9092,9202
                    require_helper_t_stringliteral_50b159bbb975f5448705db79eafd212ba91c20fe5a110a13759239545d3339af(expr_1137)
                    /// @src 2:9223,9230
                    let expr_1142_functionIdentifier := 492
                    /// @src 2:9231,9239
                    let _256 := var_balance0_966
                    let expr_1143 := _256
                    /// @src 2:9241,9249
                    let _257 := var_balance1_969
                    let expr_1144 := _257
                    /// @src 2:9251,9260
                    let _258 := var__reserve0_948
                    let expr_1145 := _258
                    /// @src 2:9262,9271
                    let _259 := var__reserve1_950
                    let expr_1146 := _259
                    fun__update_492(expr_1143, expr_1144, expr_1145, expr_1146)
                    /// @src 2:9292,9302
                    let expr_1151 := caller()
                    /// @src 2:9304,9313
                    let _260 := var_amount0In_1052
                    let expr_1152 := _260
                    /// @src 2:9315,9324
                    let _261 := var_amount1In_1068
                    let expr_1153 := _261
                    /// @src 2:9326,9336
                    let _262 := var_amount0Out_924
                    let expr_1154 := _262
                    /// @src 2:9338,9348
                    let _263 := var_amount1Out_926
                    let expr_1155 := _263
                    /// @src 2:9350,9352
                    let _264 := var_to_928
                    let expr_1156 := _264
                    /// @src 2:9287,9353
                    let _265 := 0xd78ad95fa46c994b6551d0da85fc275fe613ce37657fb8d5e3d130840159d822
                    {
                        let _266 := allocate_unbounded()
                        let _267 := abi_encode_tuple_t_uint256_t_uint256_t_uint256_t_uint256__to_t_uint256_t_uint256_t_uint256_t_uint256__fromStack(_266 , expr_1152, expr_1153, expr_1154, expr_1155)
                        log3(_266, sub(_267, _266) , _265, expr_1151, expr_1156)
                    }
                }

                function fun_sync_1241() {
                    /// @src 2:9790,9955

                    modifier_lock_1215()
                }

                function fun_sync_1241_inner() {
                    /// @src 2:9790,9955

                    /// @src 2:9839,9846
                    let expr_1217_functionIdentifier := 492
                    /// @src 2:9854,9860
                    let _292 := read_from_storage_split_offset_0_t_address(0x06)
                    let expr_1219 := _292
                    /// @src 2:9847,9861
                    let expr_1220_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_1219)
                    /// @src 2:9847,9871
                    let expr_1221_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_1220_address)
                    let expr_1221_functionSelector := 0x70a08231
                    /// @src 2:9880,9884
                    let expr_1224_address := address()
                    /// @src 2:9872,9885
                    let expr_1225 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_1224_address)
                    /// @src 2:9847,9886
                    if iszero(extcodesize(expr_1221_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _293 := allocate_unbounded()
                    mstore(_293, shift_left_224(expr_1221_functionSelector))
                    let _294 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_293, 4) , expr_1225)

                    let _295 := staticcall(gas(), expr_1221_address,  _293, sub(_294, _293), _293, 32)

                    if iszero(_295) { revert_forward_1() }

                    let expr_1226
                    if _295 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_293, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_1226 :=  abi_decode_tuple_t_uint256_fromMemory(_293, add(_293, returndatasize()))
                    }
                    /// @src 2:9895,9901
                    let _296 := read_from_storage_split_offset_0_t_address(0x07)
                    let expr_1228 := _296
                    /// @src 2:9888,9902
                    let expr_1229_address := convert_t_address_to_t_contract$_IERC20_$1781(expr_1228)
                    /// @src 2:9888,9912
                    let expr_1230_address := convert_t_contract$_IERC20_$1781_to_t_address(expr_1229_address)
                    let expr_1230_functionSelector := 0x70a08231
                    /// @src 2:9921,9925
                    let expr_1233_address := address()
                    /// @src 2:9913,9926
                    let expr_1234 := convert_t_contract$_UniswapV2Pair_$1242_to_t_address(expr_1233_address)
                    /// @src 2:9888,9927
                    if iszero(extcodesize(expr_1230_address)) { revert_error_0cc013b6b3b6beabea4e3a74a6d380f0df81852ca99887912475e1f66b2a2c20() }

                    // storage for arguments and returned data
                    let _297 := allocate_unbounded()
                    mstore(_297, shift_left_224(expr_1230_functionSelector))
                    let _298 := abi_encode_tuple_t_address__to_t_address__fromStack(add(_297, 4) , expr_1234)

                    let _299 := staticcall(gas(), expr_1230_address,  _297, sub(_298, _297), _297, 32)

                    if iszero(_299) { revert_forward_1() }

                    let expr_1235
                    if _299 {

                        // update freeMemoryPointer according to dynamic return size
                        finalize_allocation(_297, returndatasize())

                        // decode return parameters from external try-call into retVars
                        expr_1235 :=  abi_decode_tuple_t_uint256_fromMemory(_297, add(_297, returndatasize()))
                    }
                    /// @src 2:9929,9937
                    let _300 := read_from_storage_split_offset_0_t_uint112(0x08)
                    let expr_1236 := _300
                    /// @src 2:9939,9947
                    let _301 := read_from_storage_split_offset_14_t_uint112(0x08)
                    let expr_1237 := _301
                    fun__update_492(expr_1226, expr_1235, expr_1236, expr_1237)

                }

                function fun_transferFrom_1616(var_from_1563, var_to_1565, var_value_1567) -> var__1571 {
                    /// @src 0:2574,2883
                    /// @src 0:2661,2665
                    let zero_t_bool_373 := zero_value_for_split_t_bool()
                    var__1571 := zero_t_bool_373

                    /// @src 0:2681,2690
                    let _374 := 0x02
                    let expr_1573 := _374
                    /// @src 0:2691,2695
                    let _375 := var_from_1563
                    let expr_1574 := _375
                    /// @src 0:2681,2696
                    let _376 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_1573,expr_1574)
                    let _377 := _376
                    let expr_1575 := _377
                    /// @src 0:2697,2707
                    let expr_1577 := caller()
                    /// @src 0:2681,2708
                    let _378 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1575,expr_1577)
                    let _379 := read_from_storage_split_offset_0_t_uint256(_378)
                    let expr_1578 := _379
                    /// @src 0:2722,2723
                    let expr_1583 := 0x01
                    /// @src 0:2721,2723
                    let expr_1584 := 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
                    /// @src 0:2717,2724
                    let expr_1585 := convert_t_rational_minus_1_by_1_to_t_int256(expr_1584)
                    /// @src 0:2712,2725
                    let expr_1586 := convert_t_int256_to_t_uint256(expr_1585)
                    /// @src 0:2681,2725
                    let expr_1587 := iszero(eq(cleanup_t_uint256(expr_1578), cleanup_t_uint256(expr_1586)))
                    /// @src 0:2677,2820
                    if expr_1587 {
                        /// @src 0:2771,2780
                        let _380 := 0x02
                        let expr_1594 := _380
                        /// @src 0:2781,2785
                        let _381 := var_from_1563
                        let expr_1595 := _381
                        /// @src 0:2771,2786
                        let _382 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_1594,expr_1595)
                        let _383 := _382
                        let expr_1596 := _383
                        /// @src 0:2787,2797
                        let expr_1598 := caller()
                        /// @src 0:2771,2798
                        let _384 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1596,expr_1598)
                        let _385 := read_from_storage_split_offset_0_t_uint256(_384)
                        let expr_1599 := _385
                        /// @src 0:2771,2802
                        let expr_1600_self := expr_1599
                        let expr_1600_functionIdentifier := 2206
                        /// @src 0:2803,2808
                        let _386 := var_value_1567
                        let expr_1601 := _386
                        /// @src 0:2771,2809
                        let expr_1602 := fun_sub_2206(expr_1600_self, expr_1601)
                        /// @src 0:2741,2750
                        let _387 := 0x02
                        let expr_1588 := _387
                        /// @src 0:2751,2755
                        let _388 := var_from_1563
                        let expr_1589 := _388
                        /// @src 0:2741,2756
                        let _389 := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(expr_1588,expr_1589)
                        let _390 := _389
                        let expr_1592 := _390
                        /// @src 0:2757,2767
                        let expr_1591 := caller()
                        /// @src 0:2741,2768
                        let _391 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_1592,expr_1591)
                        /// @src 0:2741,2809
                        update_storage_value_offset_0t_uint256_to_t_uint256(_391, expr_1602)
                        let expr_1603 := expr_1602
                        /// @src 0:2677,2820
                    }
                    /// @src 0:2829,2838
                    let expr_1607_functionIdentifier := 1521
                    /// @src 0:2839,2843
                    let _392 := var_from_1563
                    let expr_1608 := _392
                    /// @src 0:2845,2847
                    let _393 := var_to_1565
                    let expr_1609 := _393
                    /// @src 0:2849,2854
                    let _394 := var_value_1567
                    let expr_1610 := _394
                    fun__transfer_1521(expr_1608, expr_1609, expr_1610)
                    /// @src 0:2872,2876
                    let expr_1613 := 0x01
                    /// @src 0:2865,2876
                    var__1571 := expr_1613
                    leave

                }

                function fun_transfer_1561(var_to_1543, var_value_1545) -> var__1549 {
                    /// @src 0:2423,2568
                    /// @src 0:2492,2496
                    let zero_t_bool_348 := zero_value_for_split_t_bool()
                    var__1549 := zero_t_bool_348

                    /// @src 0:2508,2517
                    let expr_1551_functionIdentifier := 1521
                    /// @src 0:2518,2528
                    let expr_1553 := caller()
                    /// @src 0:2530,2532
                    let _349 := var_to_1543
                    let expr_1554 := _349
                    /// @src 0:2534,2539
                    let _350 := var_value_1545
                    let expr_1555 := _350
                    fun__transfer_1521(expr_1553, expr_1554, expr_1555)
                    /// @src 0:2557,2561
                    let expr_1558 := 0x01
                    /// @src 0:2550,2561
                    var__1549 := expr_1558
                    leave

                }

                function fun_uqdiv_2041(var_x_2024, var_y_2026) -> var_z_2029 {
                    /// @src 10:468,574
                    /// @src 10:528,537
                    let zero_t_uint224_443 := zero_value_for_split_t_uint224()
                    var_z_2029 := zero_t_uint224_443

                    /// @src 10:553,554
                    let _444 := var_x_2024
                    let expr_2032 := _444
                    /// @src 10:565,566
                    let _445 := var_y_2026
                    let expr_2035 := _445
                    /// @src 10:557,567
                    let expr_2036 := convert_t_uint112_to_t_uint224(expr_2035)
                    /// @src 10:553,567
                    let expr_2037 := checked_div_t_uint224(expr_2032, expr_2036)

                    /// @src 10:549,567
                    var_z_2029 := expr_2037
                    let expr_2038 := expr_2037

                }

                function getter_fun_DOMAIN_SEPARATOR_1344() -> ret {
                    /// @src 0:516,556

                    let slot := 3
                    let offset := 0

                    ret := read_from_storage_split_dynamic_t_bytes32(slot, offset)

                }

                function getter_fun_MINIMUM_LIQUIDITY_220() -> ret_0 {
                    /// @src 2:416,471
                    ret_0 := constant_MINIMUM_LIQUIDITY_220()
                }

                function getter_fun_PERMIT_TYPEHASH_1348() -> ret_0 {
                    /// @src 0:666,783
                    ret_0 := constant_PERMIT_TYPEHASH_1348()
                }

                function getter_fun_allowance_1341(key_0, key_1) -> ret {
                    /// @src 0:439,509

                    let slot := 2
                    let offset := 0

                    slot := mapping_index_access_t_mapping$_t_address_$_t_mapping$_t_address_$_t_uint256_$_$_of_t_address(slot, key_0)

                    slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_1)

                    ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

                }

                function getter_fun_balanceOf_1334(key_0) -> ret {
                    /// @src 0:383,433

                    let slot := 1
                    let offset := 0

                    slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_0)

                    ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

                }

                function getter_fun_decimals_1326() -> ret_0 {
                    /// @src 0:295,339
                    ret_0 := constant_decimals_1326()
                }

                function getter_fun_factory_234() -> ret {
                    /// @src 2:572,603

                    let slot := 5
                    let offset := 0

                    ret := read_from_storage_split_dynamic_t_address(slot, offset)

                }

                function getter_fun_kLast_255() -> ret {
                    /// @src 2:1065,1091

                    let slot := 11
                    let offset := 0

                    ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

                }

                function getter_fun_name_1318() -> ret_0 {
                    /// @src 0:183,234
                    ret_0 := constant_name_1318()
                }

                function getter_fun_nonces_1353(key_0) -> ret {
                    /// @src 0:789,836

                    let slot := 4
                    let offset := 0

                    slot := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot, key_0)

                    ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

                }

                function getter_fun_price0CumulativeLast_249() -> ret {
                    /// @src 2:971,1012

                    let slot := 9
                    let offset := 0

                    ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

                }

                function getter_fun_price1CumulativeLast_252() -> ret {
                    /// @src 2:1018,1059

                    let slot := 10
                    let offset := 0

                    ret := read_from_storage_split_dynamic_t_uint256(slot, offset)

                }

                function getter_fun_symbol_1322() -> ret_0 {
                    /// @src 0:240,289
                    ret_0 := constant_symbol_1322()
                }

                function getter_fun_token0_237() -> ret {
                    /// @src 2:609,639

                    let slot := 6
                    let offset := 0

                    ret := read_from_storage_split_dynamic_t_address(slot, offset)

                }

                function getter_fun_token1_240() -> ret {
                    /// @src 2:645,675

                    let slot := 7
                    let offset := 0

                    ret := read_from_storage_split_dynamic_t_address(slot, offset)

                }

                function getter_fun_totalSupply_1329() -> ret {
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

                function modifier_lock_1166(var_to_1162) {
                    /// @src 2:1209,1340

                    /// @src 2:1243,1251
                    let _268 := read_from_storage_split_offset_0_t_uint256(0x0c)
                    let expr_261 := _268
                    /// @src 2:1255,1256
                    let expr_262 := 0x01
                    /// @src 2:1243,1256
                    let expr_263 := eq(cleanup_t_uint256(expr_261), convert_t_rational_1_by_1_to_t_uint256(expr_262))
                    /// @src 2:1235,1278
                    require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_263)
                    /// @src 2:1299,1300
                    let expr_268 := 0x00
                    /// @src 2:1288,1300
                    let _269 := convert_t_rational_0_by_1_to_t_uint256(expr_268)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _269)
                    let expr_269 := _269
                    /// @src 2:1310,1311
                    fun_skim_1211_inner(var_to_1162)
                    /// @src 2:1332,1333
                    let expr_273 := 0x01
                    /// @src 2:1321,1333
                    let _270 := convert_t_rational_1_by_1_to_t_uint256(expr_273)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _270)
                    let expr_274 := _270

                }

                function modifier_lock_1215() {
                    /// @src 2:1209,1340

                    /// @src 2:1243,1251
                    let _289 := read_from_storage_split_offset_0_t_uint256(0x0c)
                    let expr_261 := _289
                    /// @src 2:1255,1256
                    let expr_262 := 0x01
                    /// @src 2:1243,1256
                    let expr_263 := eq(cleanup_t_uint256(expr_261), convert_t_rational_1_by_1_to_t_uint256(expr_262))
                    /// @src 2:1235,1278
                    require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_263)
                    /// @src 2:1299,1300
                    let expr_268 := 0x00
                    /// @src 2:1288,1300
                    let _290 := convert_t_rational_0_by_1_to_t_uint256(expr_268)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _290)
                    let expr_269 := _290
                    /// @src 2:1310,1311
                    fun_sync_1241_inner()
                    /// @src 2:1332,1333
                    let expr_273 := 0x01
                    /// @src 2:1321,1333
                    let _291 := convert_t_rational_1_by_1_to_t_uint256(expr_273)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _291)
                    let expr_274 := _291

                }

                function modifier_lock_606(var_liquidity_608, var_to_602) -> _16 {
                    /// @src 2:1209,1340
                    _16 := var_liquidity_608

                    /// @src 2:1243,1251
                    let _17 := read_from_storage_split_offset_0_t_uint256(0x0c)
                    let expr_261 := _17
                    /// @src 2:1255,1256
                    let expr_262 := 0x01
                    /// @src 2:1243,1256
                    let expr_263 := eq(cleanup_t_uint256(expr_261), convert_t_rational_1_by_1_to_t_uint256(expr_262))
                    /// @src 2:1235,1278
                    require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_263)
                    /// @src 2:1299,1300
                    let expr_268 := 0x00
                    /// @src 2:1288,1300
                    let _18 := convert_t_rational_0_by_1_to_t_uint256(expr_268)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _18)
                    let expr_269 := _18
                    /// @src 2:1310,1311
                    _16 := fun_mint_750_inner(var_liquidity_608, var_to_602)
                    /// @src 2:1332,1333
                    let expr_273 := 0x01
                    /// @src 2:1321,1333
                    let _19 := convert_t_rational_1_by_1_to_t_uint256(expr_273)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _19)
                    let expr_274 := _19

                }

                function modifier_lock_756(var_amount0_758, var_amount1_760, var_to_752) -> _116, _117 {
                    /// @src 2:1209,1340
                    _116 := var_amount0_758
                    _117 := var_amount1_760

                    /// @src 2:1243,1251
                    let _118 := read_from_storage_split_offset_0_t_uint256(0x0c)
                    let expr_261 := _118
                    /// @src 2:1255,1256
                    let expr_262 := 0x01
                    /// @src 2:1243,1256
                    let expr_263 := eq(cleanup_t_uint256(expr_261), convert_t_rational_1_by_1_to_t_uint256(expr_262))
                    /// @src 2:1235,1278
                    require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_263)
                    /// @src 2:1299,1300
                    let expr_268 := 0x00
                    /// @src 2:1288,1300
                    let _119 := convert_t_rational_0_by_1_to_t_uint256(expr_268)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _119)
                    let expr_269 := _119
                    /// @src 2:1310,1311
                    _116, _117 := fun_burn_922_inner(var_amount0_758, var_amount1_760, var_to_752)
                    /// @src 2:1332,1333
                    let expr_273 := 0x01
                    /// @src 2:1321,1333
                    let _120 := convert_t_rational_1_by_1_to_t_uint256(expr_273)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _120)
                    let expr_274 := _120

                }

                function modifier_lock_934(var_amount0Out_924, var_amount1Out_926, var_to_928, var_data_930_offset, var_data_930_length) {
                    /// @src 2:1209,1340

                    /// @src 2:1243,1251
                    let _187 := read_from_storage_split_offset_0_t_uint256(0x0c)
                    let expr_261 := _187
                    /// @src 2:1255,1256
                    let expr_262 := 0x01
                    /// @src 2:1243,1256
                    let expr_263 := eq(cleanup_t_uint256(expr_261), convert_t_rational_1_by_1_to_t_uint256(expr_262))
                    /// @src 2:1235,1278
                    require_helper_t_stringliteral_4cc87f075f04bdfaccb0dc54ec0b98f9169b1507a7e83ec8ee97e34d6a77db4a(expr_263)
                    /// @src 2:1299,1300
                    let expr_268 := 0x00
                    /// @src 2:1288,1300
                    let _188 := convert_t_rational_0_by_1_to_t_uint256(expr_268)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _188)
                    let expr_269 := _188
                    /// @src 2:1310,1311
                    fun_swap_1160_inner(var_amount0Out_924, var_amount1Out_926, var_to_928, var_data_930_offset, var_data_930_length)
                    /// @src 2:1332,1333
                    let expr_273 := 0x01
                    /// @src 2:1321,1333
                    let _189 := convert_t_rational_1_by_1_to_t_uint256(expr_273)
                    update_storage_value_offset_0t_uint256_to_t_uint256(0x0c, _189)
                    let expr_274 := _189

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

    }

    data ".metadata" hex"a2646970667358221220237e10a8f0685a392a732be11857e65bc2e8672db67b6fe5f7606aa7f0bf7fed64736f6c63430008060033"
}

