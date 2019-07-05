#pragma once

#include <memory>
#include <vector>

namespace soll {

using StmtPtr = std::unique_ptr<Stmt>;

class Stmt {

};

class DeclStmt : public Stmt {

};

class ExprStmt : public Stmt {
};

class Block : public Stmt {
    std::vector<StmtPtr> stmts;
};

class IfStmt : public Stmt {
    StmtPtr m_condition;
	StmtPtr m_trueBody;
	StmtPtr m_falseBody; ///< "else" part, optional
};

class BreakableStmt : public Stmt {
};

class ForStmt : public Stmt
{
};

class ContinueStmt : public Stmt
{
};

class BreakStmt : public Stmt
{
};

class ReturnStmt : public Stmt
{
};

}