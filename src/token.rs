use std::fmt;

pub enum Token {
    WhiteSpace,
    Id(String),
    True,
    False,
    While,
    If,
    ElseIf,
    Else,
    Break,
    Return,
    IntT(String),
    BooleanT(String),
    StringT(String),
    VoidT,
    Function,
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
    LogicAndOp,
    LogicOrOp,
}

impl fmt::Display for Token {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Token::Id(id) => write!(f, "{}", id),
            Token::IntT(i) => write!(f, "{}", i),
            Token::BooleanT(b) => write!(f, "{}", b),
            Token::StringT(s) => write!(f, "{}", s),
            Token::Function => write!(f, "fun"),
            Token::WhiteSpace => write!(f, " "),
            Token::True => write!(f, "true"),
            Token::False => write!(f, "false"),
            Token::While => write!(f, "while"),
            Token::Return => write!(f, "return"),
            Token::Break => write!(f, "break"),
            Token::If => write!(f, "if"),
            Token::ElseIf => write!(f, "else if"),
            Token::Else => write!(f, "else"),
            Token::LParen => write!(f, "("),
            Token::RParen => write!(f, "("),
            Token::LBrace => write!(f, "{{"),
            Token::RBrace => write!(f, "}}"),
            Token::Semi => write!(f, ";"),
            Token::Comma => write!(f, ","),
            Token::AddOp => write!(f, "+"),
            Token::SubOp => write!(f, "-"),
            Token::MulOp => write!(f, "*"),
            Token::DivOp => write!(f, "/"),
            Token::ModOp => write!(f, "%"),
            Token::GtOp => write!(f, ">"),
            Token::LtOp => write!(f, "<"),
            Token::GtEqOp => write!(f, ">="),
            Token::LtEqOp => write!(f, "<="),
            Token::EqOp => write!(f, "="),
            Token::EqEqOp => write!(f, "=="),
            Token::NotOp => write!(f, "!"),
            Token::NotEqOp => write!(f, "!="),
            Token::LogicAndOp => write!(f, "&&"),
            Token::LogicOrOp => write!(f, "||"),

            _ => write!(f, "unknown token"),
        }
    }
}

pub fn keyword_to_token(keyword: &str) -> Option<Token> {
    match keyword {
        "fun" => Some(Token::Function),
        "if" => Some(Token::If),
        "else if" => Some(Token::ElseIf),
        "else" => Some(Token::Else),
        "while" => Some(Token::While),
        "return" => Some(Token::Return),
        "break" => Some(Token::Break),
        "void" => Some(Token::VoidT),
        "true" => Some(Token::True),
        "false" => Some(Token::False),
        _ => None,
    }
}

#[test]
fn test_keyword_to_token_should_pass() {
    assert!(keyword_to_token("fun").is_some());
}

#[test]
fn test_keyword_to_token_should_fail() {
    assert!(keyword_to_token("function").is_none());
}
