// RUN: %soll --lang=Yul %s
object "a" {
    code {
        setimmutable(
            10,
            "imm2",
            address()
        )
        let memPtr := mload(64)
        codecopy(memPtr, dataoffset("Test_deployed"), datasize("Test_deployed"))
        setimmutable(
            200,
            "imm1",
            address()
        )
        return(memPtr, datasize("Test_deployed"))
    }
    object "Test_deployed" {
        code {
            let addr := loadimmutable("imm1")
            return(addr, 20)
            function fun() -> temp {
                let temp2 := loadimmutable("imm2")
                temp := loadimmutable("imm1")
            }
        }
    }
}
