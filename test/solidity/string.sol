pragma solidity ^0.5.0;
contract StringTest {
  string public str1;
  string public str2;
  string public str3;
  mapping (string => string) private str_map;
  constructor() public {
    str1 = "SecondState";
    str2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";
    str3 = str2;
    str_map[str3] = str1;
  }
}
