use std::fmt;

pub enum TokenType {
    WhiteSpace,
    True,
    False,
    Number,
    While,
    If,
    Else,
    Break,
    Return,
    Int,
    Boolean,
    Void,
    StringT,
    LParen,
    RParen,
    LBrace,
    RBrace,
    Semi,
    Comma,
    AddOp,
    SubOp,
    MulOp,
    DivOp,
    ModOp,
    GtOp,
    LtOp,
    GtEqOp,
    LtEqOp,
    EqOp,
    EqEqOp,
    NotOp,
    NotEqOp,
    AndOp,
    OrOp
}

impl fmt::Display for TokenType {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            TokenType::WhiteSpace => write!(f, " "),
            TokenType::True => write!(f, "true"),
            _ => write!(f, "unknown token")
        }
    }
}

pub struct Token {
    token_type: TokenType,
    val: String,
}
