#include "CodeGenModule.h"

namespace soll {

CodeGenModule::CodeGenModule(llvm::Module& module)
    : module{module}
{
}

} // namespace soll
