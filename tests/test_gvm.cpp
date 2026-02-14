#include "Core/GVM.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>

using namespace Core;

// Helper to append integer to bytecode
void AppendInt(std::vector<uint8_t>& code, int64_t val) {
    uint8_t bytes[8];
    std::memcpy(bytes, &val, 8);
    for (int i = 0; i < 8; ++i) code.push_back(bytes[i]);
}

void AppendInt32(std::vector<uint8_t>& code, int32_t val) {
    uint8_t bytes[4];
    std::memcpy(bytes, &val, 4);
    for (int i = 0; i < 4; ++i) code.push_back(bytes[i]);
}

void TestMath() {
    std::cout << "Testing Math..." << std::endl;
    GVM vm;
    VMContext ctx;
    
    std::vector<uint8_t> code;
    
    // PUSH 10
    code.push_back(static_cast<uint8_t>(Opcode::PUSH));
    AppendInt(code, 10);
    
    // PUSH 20
    code.push_back(static_cast<uint8_t>(Opcode::PUSH));
    AppendInt(code, 20);
    
    // ADD (20 + 10 = 30)
    code.push_back(static_cast<uint8_t>(Opcode::ADD));
    
    // HALT
    code.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    bool load = vm.Load(code);
    assert(load);
    
    bool exec = vm.Execute(ctx);
    if (!exec) std::cout << "Error: " << ctx.error << std::endl;
    assert(exec);
    
    assert(ctx.sp == 1);
    assert(ctx.stack[0] == 30);
    std::cout << "Math PASSED." << std::endl;
}

void TestExternal() {
    std::cout << "Testing External Call..." << std::endl;
    GVM vm;
    VMContext ctx;
    
    // Define external function: Double the input
    vm.RegisterFunction(1, [](const std::vector<GVMInt>& args) -> GVMInt {
        return args[0] * 2;
    }, {1, true});
    
    std::vector<uint8_t> code;
    
    // PUSH 5
    code.push_back(static_cast<uint8_t>(Opcode::PUSH));
    AppendInt(code, 5);
    
    // CALL 1
    code.push_back(static_cast<uint8_t>(Opcode::EXT_CALL));
    code.push_back(1);
    
    // HALT
    code.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    vm.Load(code);
    vm.Execute(ctx);
    
    assert(ctx.stack[0] == 10);
    std::cout << "External Call PASSED." << std::endl;
}

void TestJump() {
    std::cout << "Testing Jumps..." << std::endl;
    GVM vm;
    VMContext ctx;
    
    std::vector<uint8_t> code;
    
    // 0: PUSH 1
    code.push_back(static_cast<uint8_t>(Opcode::PUSH));
    AppendInt(code, 1);
    
    // 9: JMP +9 (Skip to HALT, skip BAD Code)
    // Instruction size: JMP(1) + 4 = 5 bytes.
    // We want to skip PUSH 999 (1+8=9 bytes)
    // Target is relative to JMP start (9). 
    // Target = 9 + Offset.
    // We want to land at...
    // Let's calculate exactly.
    // IP 0: PUSH (9 bytes) -> Next IP 9
    // IP 9: JMP (5 bytes)
    // IP 14: PUSH 999 (9 bytes) -> IP 23
    // IP 23: HALT
    
    // JMP is at 9. We want to land at 23.
    // Target = Start(9) + Offset = 23.
    // Offset = 14.
    
    code.push_back(static_cast<uint8_t>(Opcode::JMP));
    AppendInt32(code, 14); 
    
    // 14: PUSH 999 (Should be skipped)
    code.push_back(static_cast<uint8_t>(Opcode::PUSH));
    AppendInt(code, 999);
    
    // 23: HALT
    code.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    bool load = vm.Load(code);
    if (!load) std::cout << "Load Failed: Valid Jumps?" << std::endl;
    assert(load);
    
    vm.Execute(ctx);
    
    assert(ctx.stack[0] == 1); // Only 1 should be on stack
    assert(ctx.sp == 1);
    std::cout << "Jump PASSED." << std::endl;
}

void TestEFIFail_StackUnderflow() {
    std::cout << "Testing EFI Stack Underflow..." << std::endl;
    GVM vm;
    VMContext ctx;
    
    // Register ID 1, Arg Count 1
    vm.RegisterFunction(1, [](const std::vector<GVMInt>&){ return 0; }, {1, true});
    
    std::vector<uint8_t> code;
    // CALL 1 (Stack Empty)
    code.push_back(static_cast<uint8_t>(Opcode::EXT_CALL));
    code.push_back(1);
    code.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    vm.Load(code);
    bool exec = vm.Execute(ctx);
    
    assert(!exec);
    assert(ctx.error == "ExtCall Stack Underflow");
    std::cout << "EFI Stack Underflow PASSED." << std::endl;
}

void TestEFIFail_InvalidID() {
    std::cout << "Testing Invalid EFI ID..." << std::endl;
    GVM vm;
    VMContext ctx;
    
    std::vector<uint8_t> code;
    // CALL 99
    code.push_back(static_cast<uint8_t>(Opcode::EXT_CALL));
    code.push_back(99);
    code.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    vm.Load(code);
    bool exec = vm.Execute(ctx);
    
    assert(!exec);
    assert(ctx.error == "Invalid External Function Call");
    std::cout << "Invalid EFI ID PASSED." << std::endl;
}

#include "Core/Crypto.h" // Needed for HMAC test

void TestHMAC_Success() {
    std::cout << "Testing HMAC Success..." << std::endl;
    GVM vm;
    std::vector<uint8_t> code;
    code.push_back(static_cast<uint8_t>(Opcode::PUSH));
    AppendInt(code, 42);
    code.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    std::string secret = "MySecretKey";
    std::string signature = Crypto::HMAC_SHA256(secret, code);
    
    // Convert hex signature to bytes
    for (size_t i = 0; i < signature.length(); i += 2) {
        std::string byteString = signature.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
        code.push_back(byte);
    }
    
    bool load = vm.Load(code, secret);
    assert(load);
    std::cout << "HMAC Success PASSED." << std::endl;
}

void TestHMAC_Fail() {
    std::cout << "Testing HMAC Failure..." << std::endl;
    GVM vm;
    std::vector<uint8_t> code;
    code.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    // Append Fake Signature (32 bytes of zeros)
    for(int i=0; i<32; ++i) code.push_back(0);
    
    bool load = vm.Load(code, "MySecretKey");
    assert(!load);
    std::cout << "HMAC Failure PASSED." << std::endl;
}

int main() {
    TestMath();
    TestExternal();
    TestJump();
    TestEFIFail_StackUnderflow();
    TestEFIFail_InvalidID();
    TestHMAC_Success();
    TestHMAC_Fail();
    return 0;
}
