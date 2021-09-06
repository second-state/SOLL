object "Token_140" {
    code {
        mstore(64, 128)
        if callvalue() { revert(0, 0) }
        // Begin state variable initialization for contract "Token" (2 variables)
        // End state variable initialization for contract "Token".
        fun__80()


        codecopy(0, dataoffset("Token_140_deployed"), datasize("Token_140_deployed"))
        return(0, datasize("Token_140_deployed"))


        function abi_encode_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 27)

            mstore(add(pos, 0), "SafeMath: addition overflow")

            end := add(pos, 32)
        }

        function abi_encode_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862_to_t_string_memory_ptr_fromStack(pos) -> end {
            pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 30)

            mstore(add(pos, 0), "SafeMath: subtraction overflow")

            end := add(pos, 32)
        }

        function abi_encode_t_uint256_to_t_uint256_fromStack(value, pos) {
            mstore(pos, cleanup_t_uint256(value))
        }

        function abi_encode_tuple_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
            tail := add(headStart, 32)

            mstore(add(headStart, 0), sub(tail, headStart))
            tail := abi_encode_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862_to_t_string_memory_ptr_fromStack( tail)

        }

        function abi_encode_tuple_t_uint256__to_t_uint256__fromStack(headStart , value0) -> tail {
            tail := add(headStart, 32)

            abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

        }

        function array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, length) -> updated_pos {
            mstore(pos, length)
            updated_pos := add(pos, 0x20)
        }

        function checked_add_t_uint256(x, y) -> sum {

            // overflow, if x > (maxValue - y)
            if gt(x, sub(0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff, y)) { revert(0, 0) }

            sum := add(x, y)
        }

        function checked_sub_t_uint256(x, y) -> diff {

            if lt(x, y) { revert(0, 0) }

            diff := sub(x, y)
        }

        function cleanup_from_storage_t_uint256(value) -> cleaned {
            cleaned := value
        }

        function cleanup_t_uint160(value) -> cleaned {
            cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffff)
        }

        function cleanup_t_uint256(value) -> cleaned {
            cleaned := value
        }

        function convert_t_address_payable_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_address(value)
        }

        function convert_t_address_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_address(value)
        }

        function convert_t_rational_100000000_by_1_to_t_uint256(value) -> converted {
            converted := cleanup_t_uint256(value)
        }

        function convert_t_uint160_to_t_address(value) -> converted {
            converted := convert_t_uint160_to_t_uint160(value)
        }

        function convert_t_uint160_to_t_uint160(value) -> converted {
            converted := cleanup_t_uint160(value)
        }

        function extract_from_storage_value_offset_0t_uint256(slot_value) -> value {
            value := cleanup_from_storage_t_uint256(shift_right_0_unsigned(slot_value))
        }

        function fun__139()  {

        }

        function fun__80()  {
            let expr_69 := 0x05f5e100
            let _1 := convert_t_rational_100000000_by_1_to_t_uint256(expr_69)
            update_storage_value_offset_0t_uint256(0x00, _1)
            let expr_70 := _1
            let expr_76 := read_from_storage_offset_0_t_uint256(0x00)
            let _2 := expr_76
            let expr_72 := 0x01
            let expr_74 := caller()
            let _3 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address_payable(expr_72 , expr_74)
            update_storage_value_offset_0t_uint256(_3, _2)
            let expr_77 := _2

        }

        function fun_add_40(vloc_a_17, vloc_b_19)  -> vloc__22 {
            let expr_26 := vloc_a_17
            let expr_27 := vloc_b_19
            let expr_28 := checked_add_t_uint256(expr_26, expr_27)
            let vloc_c_25 := expr_28
            let expr_31 := vloc_c_25
            let expr_32 := vloc_a_17
            let expr_33 := iszero(lt(expr_31, expr_32))
            require_helper_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a(expr_33)
            let expr_37 := vloc_c_25
            vloc__22 := expr_37
            leave

        }

        function fun_balanceOf_92(vloc_account_82)  -> vloc__85 {
            let expr_87 := 0x01
            let expr_88 := vloc_account_82
            let _4 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_87 , expr_88)
            let expr_89 := read_from_storage_offset_0_t_uint256(_4)
            vloc__85 := expr_89
            leave

        }

        function fun_sub_65(vloc_a_42, vloc_b_44)  -> vloc__47 {
            let expr_50 := vloc_b_44
            let expr_51 := vloc_a_42
            let expr_52 := iszero(gt(expr_50, expr_51))
            require_helper_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862(expr_52)
            let expr_58 := vloc_a_42
            let expr_59 := vloc_b_44
            let expr_60 := checked_sub_t_uint256(expr_58, expr_59)
            let vloc_c_57 := expr_60
            let expr_62 := vloc_c_57
            vloc__47 := expr_62
            leave

        }

        function fun_transfer_135(vloc_to_94, vloc_amount_96)  -> vloc__99 {
            let expr_105 := 65
            let expr_106 := 0x01
            let expr_108 := caller()
            let _5 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address_payable(expr_106 , expr_108)
            let expr_109 := read_from_storage_offset_0_t_uint256(_5)
            let expr_110 := vloc_amount_96
            let expr_111 := fun_sub_65(expr_109, expr_110)
            let _6 := expr_111
            let expr_101 := 0x01
            let expr_103 := caller()
            let _7 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address_payable(expr_101 , expr_103)
            update_storage_value_offset_0t_uint256(_7, _6)
            let expr_112 := _6
            let expr_117 := 40
            let expr_118 := 0x01
            let expr_119 := vloc_to_94
            let _8 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_118 , expr_119)
            let expr_120 := read_from_storage_offset_0_t_uint256(_8)
            let expr_121 := vloc_amount_96
            let expr_122 := fun_add_40(expr_120, expr_121)
            let _9 := expr_122
            let expr_114 := 0x01
            let expr_115 := vloc_to_94
            let _10 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_114 , expr_115)
            update_storage_value_offset_0t_uint256(_10, _9)
            let expr_123 := _9
            let expr_127 := caller()
            let expr_128 := vloc_to_94
            let expr_129 := vloc_amount_96
            let _11 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
            let _12 := convert_t_address_payable_to_t_address(expr_127)
            let _13 := expr_128
            {
                let _14 := mload(64)
                let _15 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_14 , expr_129)
                log3(_14, sub(_15, _14) , _11, _12, _13)
            }let expr_132 := 0x01
            vloc__99 := expr_132
            leave

        }

        function mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot , key) -> dataSlot {
            mstore(0, convert_t_address_to_t_address(key))
            mstore(0x20, slot)
            dataSlot := keccak256(0, 0x40)
        }

        function mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address_payable(slot , key) -> dataSlot {
            mstore(0, convert_t_address_payable_to_t_address(key))
            mstore(0x20, slot)
            dataSlot := keccak256(0, 0x40)
        }

        function prepare_store_t_uint256(value) -> ret {
            ret := value
        }

        function read_from_storage_offset_0_t_uint256(slot) -> value {
            value := extract_from_storage_value_offset_0t_uint256(sload(slot))
        }

        function require_helper_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a(condition ) {
            if iszero(condition) {
                let fmp := mload(64)
                mstore(fmp, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a__to_t_string_memory_ptr__fromStack(add(fmp, 4) )
                revert(fmp, sub(end, fmp))
            }
        }

        function require_helper_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862(condition ) {
            if iszero(condition) {
                let fmp := mload(64)
                mstore(fmp, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                let end := abi_encode_tuple_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862__to_t_string_memory_ptr__fromStack(add(fmp, 4) )
                revert(fmp, sub(end, fmp))
            }
        }

        function shift_left_0(value) -> newValue {
            newValue :=

            shl(0, value)

        }

        function shift_right_0_unsigned(value) -> newValue {
            newValue :=

            shr(0, value)

        }

        function update_byte_slice_32_shift_0(value, toInsert) -> result {
            let mask := 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
            toInsert := shift_left_0(toInsert)
            value := and(value, not(mask))
            result := or(value, and(toInsert, mask))
        }

        function update_storage_value_offset_0t_uint256(slot, value) {
            sstore(slot, update_byte_slice_32_shift_0(sload(slot), prepare_store_t_uint256(value)))
        }


    }
    object "Token_140_deployed" {
        code {
            mstore(64, 128)

            if iszero(lt(calldatasize(), 4))
            {
                let selector := shift_right_224_unsigned(calldataload(0))
                switch selector

                case 0x70a08231
                {
                    // balanceOf(address)
                    if callvalue() { revert(0, 0) }
                    let param_0 :=  abi_decode_tuple_t_address(4, calldatasize())
                    let ret_0 :=  fun_balanceOf_92(param_0)
                    let memPos := allocateMemory(0)
                    let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos ,  ret_0)
                    return(memPos, sub(memEnd, memPos))
                }

                case 0xa9059cbb
                {
                    // transfer(address,uint256)
                    if callvalue() { revert(0, 0) }
                    let param_0, param_1 :=  abi_decode_tuple_t_addresst_uint256(4, calldatasize())
                    let ret_0 :=  fun_transfer_135(param_0, param_1)
                    let memPos := allocateMemory(0)
                    let memEnd := abi_encode_tuple_t_bool__to_t_bool__fromStack(memPos ,  ret_0)
                    return(memPos, sub(memEnd, memPos))
                }

                default {}
            }
            if iszero(calldatasize()) {  }
            fun__139() stop()


            function abi_decode_t_address(offset, end) -> value {
                value := calldataload(offset)
                validator_revert_t_address(value)
            }

            function abi_decode_t_uint256(offset, end) -> value {
                value := calldataload(offset)
                validator_revert_t_uint256(value)
            }

            function abi_decode_tuple_t_address(headStart, dataEnd) -> value0 {
                if slt(sub(dataEnd, headStart), 32) { revert(0, 0) }

                {
                    let offset := 0
                    value0 := abi_decode_t_address(add(headStart, offset), dataEnd)
                }

            }

            function abi_decode_tuple_t_addresst_uint256(headStart, dataEnd) -> value0, value1 {
                if slt(sub(dataEnd, headStart), 64) { revert(0, 0) }

                {
                    let offset := 0
                    value0 := abi_decode_t_address(add(headStart, offset), dataEnd)
                }

                {
                    let offset := 32
                    value1 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
                }

            }

            function abi_encode_t_bool_to_t_bool_fromStack(value, pos) {
                mstore(pos, cleanup_t_bool(value))
            }

            function abi_encode_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a_to_t_string_memory_ptr_fromStack(pos) -> end {
                pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 27)

                mstore(add(pos, 0), "SafeMath: addition overflow")

                end := add(pos, 32)
            }

            function abi_encode_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862_to_t_string_memory_ptr_fromStack(pos) -> end {
                pos := array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, 30)

                mstore(add(pos, 0), "SafeMath: subtraction overflow")

                end := add(pos, 32)
            }

            function abi_encode_t_uint256_to_t_uint256_fromStack(value, pos) {
                mstore(pos, cleanup_t_uint256(value))
            }

            function abi_encode_tuple_t_bool__to_t_bool__fromStack(headStart , value0) -> tail {
                tail := add(headStart, 32)

                abi_encode_t_bool_to_t_bool_fromStack(value0,  add(headStart, 0))

            }

            function abi_encode_tuple_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
                tail := add(headStart, 32)

                mstore(add(headStart, 0), sub(tail, headStart))
                tail := abi_encode_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a_to_t_string_memory_ptr_fromStack( tail)

            }

            function abi_encode_tuple_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862__to_t_string_memory_ptr__fromStack(headStart ) -> tail {
                tail := add(headStart, 32)

                mstore(add(headStart, 0), sub(tail, headStart))
                tail := abi_encode_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862_to_t_string_memory_ptr_fromStack( tail)

            }

            function abi_encode_tuple_t_uint256__to_t_uint256__fromStack(headStart , value0) -> tail {
                tail := add(headStart, 32)

                abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

            }

            function allocateMemory(size) -> memPtr {
                memPtr := mload(64)
                let newFreePtr := add(memPtr, size)
                // protect against overflow
                if or(gt(newFreePtr, 0xffffffffffffffff), lt(newFreePtr, memPtr)) { revert(0, 0) }
                mstore(64, newFreePtr)
            }

            function array_storeLengthForEncoding_t_string_memory_ptr_fromStack(pos, length) -> updated_pos {
                mstore(pos, length)
                updated_pos := add(pos, 0x20)
            }

            function checked_add_t_uint256(x, y) -> sum {

                // overflow, if x > (maxValue - y)
                if gt(x, sub(0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff, y)) { revert(0, 0) }

                sum := add(x, y)
            }

            function checked_sub_t_uint256(x, y) -> diff {

                if lt(x, y) { revert(0, 0) }

                diff := sub(x, y)
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

            function cleanup_t_uint160(value) -> cleaned {
                cleaned := and(value, 0xffffffffffffffffffffffffffffffffffffffff)
            }

            function cleanup_t_uint256(value) -> cleaned {
                cleaned := value
            }

            function convert_t_address_payable_to_t_address(value) -> converted {
                converted := convert_t_uint160_to_t_address(value)
            }

            function convert_t_address_to_t_address(value) -> converted {
                converted := convert_t_uint160_to_t_address(value)
            }

            function convert_t_rational_100000000_by_1_to_t_uint256(value) -> converted {
                converted := cleanup_t_uint256(value)
            }

            function convert_t_uint160_to_t_address(value) -> converted {
                converted := convert_t_uint160_to_t_uint160(value)
            }

            function convert_t_uint160_to_t_uint160(value) -> converted {
                converted := cleanup_t_uint160(value)
            }

            function extract_from_storage_value_offset_0t_uint256(slot_value) -> value {
                value := cleanup_from_storage_t_uint256(shift_right_0_unsigned(slot_value))
            }

            function fun__139()  {

            }

            function fun__80()  {
                let expr_69 := 0x05f5e100
                let _13 := convert_t_rational_100000000_by_1_to_t_uint256(expr_69)
                update_storage_value_offset_0t_uint256(0x00, _13)
                let expr_70 := _13
                let expr_76 := read_from_storage_offset_0_t_uint256(0x00)
                let _14 := expr_76
                let expr_72 := 0x01
                let expr_74 := caller()
                let _15 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address_payable(expr_72 , expr_74)
                update_storage_value_offset_0t_uint256(_15, _14)
                let expr_77 := _14

            }

            function fun_add_40(vloc_a_17, vloc_b_19)  -> vloc__22 {
                let expr_26 := vloc_a_17
                let expr_27 := vloc_b_19
                let expr_28 := checked_add_t_uint256(expr_26, expr_27)
                let vloc_c_25 := expr_28
                let expr_31 := vloc_c_25
                let expr_32 := vloc_a_17
                let expr_33 := iszero(lt(expr_31, expr_32))
                require_helper_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a(expr_33)
                let expr_37 := vloc_c_25
                vloc__22 := expr_37
                leave

            }

            function fun_balanceOf_92(vloc_account_82)  -> vloc__85 {
                let expr_87 := 0x01
                let expr_88 := vloc_account_82
                let _1 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_87 , expr_88)
                let expr_89 := read_from_storage_offset_0_t_uint256(_1)
                vloc__85 := expr_89
                leave

            }

            function fun_sub_65(vloc_a_42, vloc_b_44)  -> vloc__47 {
                let expr_50 := vloc_b_44
                let expr_51 := vloc_a_42
                let expr_52 := iszero(gt(expr_50, expr_51))
                require_helper_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862(expr_52)
                let expr_58 := vloc_a_42
                let expr_59 := vloc_b_44
                let expr_60 := checked_sub_t_uint256(expr_58, expr_59)
                let vloc_c_57 := expr_60
                let expr_62 := vloc_c_57
                vloc__47 := expr_62
                leave

            }

            function fun_transfer_135(vloc_to_94, vloc_amount_96)  -> vloc__99 {
                let expr_105 := 65
                let expr_106 := 0x01
                let expr_108 := caller()
                let _2 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address_payable(expr_106 , expr_108)
                let expr_109 := read_from_storage_offset_0_t_uint256(_2)
                let expr_110 := vloc_amount_96
                let expr_111 := fun_sub_65(expr_109, expr_110)
                let _3 := expr_111
                let expr_101 := 0x01
                let expr_103 := caller()
                let _4 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address_payable(expr_101 , expr_103)
                update_storage_value_offset_0t_uint256(_4, _3)
                let expr_112 := _3
                let expr_117 := 40
                let expr_118 := 0x01
                let expr_119 := vloc_to_94
                let _5 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_118 , expr_119)
                let expr_120 := read_from_storage_offset_0_t_uint256(_5)
                let expr_121 := vloc_amount_96
                let expr_122 := fun_add_40(expr_120, expr_121)
                let _6 := expr_122
                let expr_114 := 0x01
                let expr_115 := vloc_to_94
                let _7 := mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(expr_114 , expr_115)
                update_storage_value_offset_0t_uint256(_7, _6)
                let expr_123 := _6
                let expr_127 := caller()
                let expr_128 := vloc_to_94
                let expr_129 := vloc_amount_96
                let _8 := 0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef
                let _9 := convert_t_address_payable_to_t_address(expr_127)
                let _10 := expr_128
                {
                    let _11 := mload(64)
                    let _12 := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(_11 , expr_129)
                    log3(_11, sub(_12, _11) , _8, _9, _10)
                }let expr_132 := 0x01
                vloc__99 := expr_132
                leave

            }

            function mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address(slot , key) -> dataSlot {
                mstore(0, convert_t_address_to_t_address(key))
                mstore(0x20, slot)
                dataSlot := keccak256(0, 0x40)
            }

            function mapping_index_access_t_mapping$_t_address_$_t_uint256_$_of_t_address_payable(slot , key) -> dataSlot {
                mstore(0, convert_t_address_payable_to_t_address(key))
                mstore(0x20, slot)
                dataSlot := keccak256(0, 0x40)
            }

            function prepare_store_t_uint256(value) -> ret {
                ret := value
            }

            function read_from_storage_offset_0_t_uint256(slot) -> value {
                value := extract_from_storage_value_offset_0t_uint256(sload(slot))
            }

            function require_helper_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a(condition ) {
                if iszero(condition) {
                    let fmp := mload(64)
                    mstore(fmp, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                    let end := abi_encode_tuple_t_stringliteral_30cc447bcc13b3e22b45cef0dd9b0b514842d836dd9b6eb384e20dedfb47723a__to_t_string_memory_ptr__fromStack(add(fmp, 4) )
                    revert(fmp, sub(end, fmp))
                }
            }

            function require_helper_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862(condition ) {
                if iszero(condition) {
                    let fmp := mload(64)
                    mstore(fmp, 0x08c379a000000000000000000000000000000000000000000000000000000000)
                    let end := abi_encode_tuple_t_stringliteral_50b058e9b5320e58880d88223c9801cd9eecdcf90323d5c2318bc1b6b916e862__to_t_string_memory_ptr__fromStack(add(fmp, 4) )
                    revert(fmp, sub(end, fmp))
                }
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

            function update_byte_slice_32_shift_0(value, toInsert) -> result {
                let mask := 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
                toInsert := shift_left_0(toInsert)
                value := and(value, not(mask))
                result := or(value, and(toInsert, mask))
            }

            function update_storage_value_offset_0t_uint256(slot, value) {
                sstore(slot, update_byte_slice_32_shift_0(sload(slot), prepare_store_t_uint256(value)))
            }


            function validator_revert_t_address(value) {
                if iszero(eq(value, cleanup_t_address(value))) { revert(0, 0) }
            }

            function validator_revert_t_uint256(value) {
                if iszero(eq(value, cleanup_t_uint256(value))) { revert(0, 0) }
            }

        }
    }
}


