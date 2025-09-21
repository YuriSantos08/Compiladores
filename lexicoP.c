#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {

    // OPERADDDORES
    TOKEN_EQ,      // ==
    TOKEN_GE,     // >=
    TOKEN_MUL,   // *
    TOKEN_NE,    // <>
    TOKEN_LE,  // <=
    TOKEN_DIV, // /                                             
    TOKEN_GT,  // >
    TOKEN_AD,   // +
    TOKEN_ASS,  // :=
    TOKEN_LT,   // <
    TOKEN_MIN,  // -
    
    //SIMBOLOS
    TOKEN_OBC,  // {
    TOKEN_COM,   // ,
    TOKEN_CBC, // }                                            
    TOKEN_SEM,  // ;
    TOKEN_OPA,  // (
    TOKEN_CPA, // )
    TOKEN_DP, // :
    TOKEN_PONTO, // .

    // Palavras-chave
    TOKEN_PROGRAM,
    TOKEN_VAR,
    TOKEN_INTEGER,
    TOKEN_REAL,
    TOKEN_BEGIN,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_DO,

     // Outros
    TOKEN_ID,       // Identificadores
    TOKEN_NUM_INT,  // Números inteiros
    TOKEN_NUM_REAL, // Números reais
    TOKEN_ERRO,     // Erros léxicos
    TOKEN_FIM       // Fim de arquivo
} TipoToken;

typedef struct {
    TipoToken tipo;
    char *lexema;
    int linha;
    int coluna;
} Token;

typedef struct {
    const char *src;
    int i;
    int linha, coluna;
    char c;
} Scanner;

TipoToken palavra_ou_id(const char *lexema);

char* nome_token(TipoToken t){
    switch(t){

        case TOKEN_EQ:      return "IGUAL_COMPARAÇÃO";
        case TOKEN_GE:      return "MAIOR_IGUAL";
        case TOKEN_MUL:     return "MULTIPLICAÇÃO";
        case TOKEN_NE:      return "DIFERENTE_DE";
        case TOKEN_LE:      return "MENOR_IGUAL";
        case TOKEN_DIV:     return "DIVISÃO";
        case TOKEN_GT:      return "MAIOR_QUE";
        case TOKEN_AD:      return "ADIÇÃO";
        case TOKEN_ASS:     return "IGUAL_ATRIBUIÇÃO";
        case TOKEN_LT:      return "MENOR_QUE";
        case TOKEN_MIN:     return "MENOS";
        case TOKEN_OBC:     return "ABRE_CHAVES";
        case TOKEN_COM:     return "VIRGULA";
        case TOKEN_CBC:     return "FECHA_CHAVES";
        case TOKEN_SEM:     return "PONTO_VIRGULA";
        case TOKEN_OPA:     return "ABRE_PARENTESES";
        case TOKEN_CPA:     return "FECHA_PARENTESES";
        case TOKEN_DP:      return "DOIS_PONTOS";
        case TOKEN_PONTO:   return "PONTO_FINAL";

        case TOKEN_PROGRAM: return "PROGRAM";
        case TOKEN_VAR:     return "VAR";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_REAL:    return "REAL";
        case TOKEN_BEGIN:   return "BEGIN";
        case TOKEN_END:     return "END";
        case TOKEN_IF:      return "IF";
        case TOKEN_THEN:    return "THEN";
        case TOKEN_ELSE:    return "ELSE";
        case TOKEN_WHILE:   return "WHILE";
        case TOKEN_DO:      return "DO";

        case TOKEN_ID:      return "IDENTIFICADOR";
        case TOKEN_NUM_INT: return "NUM_INTEIRO";
        case TOKEN_NUM_REAL: return "NUM_REAL";
        case TOKEN_FIM:       return "FIM_DA_LINHA";
        case TOKEN_ERRO:      return "ERRO";
        default:              return "?";
    }
}

    char *str_ndup(const char *s, size_t n){
    char *p = (char*)malloc(n+1);
    if(!p){ fprintf(stderr,"Memória insuficiente\n"); exit(1); }
    memcpy(p, s, n); p[n]='\0'; return p;
}

void iniciar(Scanner *sc, const char *texto, int LinhaAntiga){
    sc->src = texto ? texto : "";
    sc->i = 0; 
    sc->linha = LinhaAntiga; 
    sc->coluna = 1;
    sc->c = sc->src[0];
}

void avancar(Scanner *sc){
    if(sc->c=='\0') return;
    if(sc->c=='\n'){ sc->linha++; sc->coluna=1; }
    else           { sc->coluna++; }
    sc->i++;
    sc->c = sc->src[sc->i];
}

void pular_espacos(Scanner *sc){
    while(isspace((unsigned char)sc->c)) avancar(sc);
}

Token criar_token_texto(Scanner *sc, TipoToken tipo, const char *ini, size_t n, int lin, int col){
    (void)sc;
    Token t; t.tipo=tipo;
    t.lexema=str_ndup(ini,n); 
    t.linha=lin; t.coluna=col; 
    return t;
}

Token token_simples(Scanner *sc, TipoToken tipo){
    int lin=sc->linha, col=sc->coluna;
    char ch=sc->c; avancar(sc);
    return criar_token_texto(sc, tipo, &ch, 1, lin, col);
}

Token token_erro_msg(Scanner *sc, const char *msg){
    return criar_token_texto(sc, TOKEN_ERRO, msg, strlen(msg), sc->linha, sc->coluna);
}

Token coletar_inteiro(Scanner *sc){
    int lin=sc->linha, col=sc->coluna;
    size_t ini = sc->i;
    if(!isdigit((unsigned char)sc->c)) return token_erro_msg(sc, "Inteiro malformado");
    while(isdigit((unsigned char)sc->c)) avancar(sc);
    return criar_token_texto(sc, TOKEN_NUM_INT, sc->src+ini, sc->i-ini, lin, col);
}

Token coletar_numero(Scanner *sc); 

Token proximo_token(Scanner *sc){
    pular_espacos(sc);

    if(sc->c=='\0') return criar_token_texto(sc, TOKEN_FIM, "", 0, sc->linha, sc->coluna);

    if(sc->c == ':') {
        int lin = sc->linha;
        int col = sc->coluna;
        avancar(sc);   
        if(sc->c == '=') {
            avancar(sc); 
            return criar_token_texto(sc, TOKEN_ASS, ":=", 2, lin, col);
        } else {
            return criar_token_texto(sc, TOKEN_DP, ":", 1, lin, col); 
        }
    }

if(sc->c=='>') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    if(sc->c == '=') {
        avancar(sc);
        return criar_token_texto(sc, TOKEN_GE, ">=", 2, lin, col);
    } else {
        return criar_token_texto(sc, TOKEN_GT, ">", 1, lin, col);
    }
}

if(sc->c == '.') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_PONTO, ".", 1, lin, col);
}

  if(sc->c == '=') {  
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);   
    return criar_token_texto(sc, TOKEN_EQ, "=", 1, lin, col);
}


    if(sc->c == '<') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    if(sc->c == '=') {
        avancar(sc);
        return criar_token_texto(sc, TOKEN_LE, "<=", 2 , lin, col);
    } else if(sc->c == '>') {
        avancar(sc);
        return criar_token_texto(sc, TOKEN_NE, "<>", 2 , lin, col);
    } else {
        return criar_token_texto(sc, TOKEN_LT, "<", 1, lin, col);
    }
}
    
    if(sc->c=='+') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_AD, "+", 1, lin, col);
}

if(sc->c=='-') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_MIN, "-", 1, lin, col);
}

if(sc->c=='*') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_MUL, "*", 1, lin, col);
}

if(sc->c=='/') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_DIV, "/", 1, lin, col);
}



if(sc->c==';') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_SEM, ";", 1, lin, col);
}

if(sc->c==',') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_COM, ",", 1, lin, col);
}

if(sc->c=='(') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_OPA, "(", 1, lin, col);
}

if(sc->c==')') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_CPA, ")", 1, lin, col);
}

if(sc->c=='{') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_OBC, "{", 1, lin, col);
}

if(sc->c=='}') {
    int lin = sc->linha;
    int col = sc->coluna;
    avancar(sc);
    return criar_token_texto(sc, TOKEN_CBC, "}", 1, lin, col);
}
    

    if (isdigit((unsigned char)sc->c)) {
    return coletar_numero(sc);
}


if (isalpha((unsigned char)sc->c)) {
    int lin = sc->linha, col = sc->coluna;
    size_t ini = sc->i;

    while (isalnum((unsigned char)sc->c)) avancar(sc);

    char *lex = str_ndup(sc->src + ini, sc->i - ini);
    TipoToken tipo = palavra_ou_id(lex);
    Token t = criar_token_texto(sc, tipo, lex, strlen(lex), lin, col);
    free(lex);
    return t;
}

return token_erro_msg(sc, "Caractere desconhecido");

}

Token coletar_numero(Scanner *sc) {
    int lin = sc->linha, col = sc->coluna;
    size_t ini = sc->i;
    int tem_ponto = 0;

    while (isdigit((unsigned char)sc->c) || sc->c == '.') {
        if (sc->c == '.') {
            if (tem_ponto) break; // só pode ter 1 ponto
            tem_ponto = 1;
        }
        avancar(sc);
    }

    if (tem_ponto)
        return criar_token_texto(sc, TOKEN_NUM_REAL, sc->src+ini, sc->i-ini, lin, col);
    else
        return criar_token_texto(sc, TOKEN_NUM_INT, sc->src+ini, sc->i-ini, lin, col);
}


TipoToken palavra_ou_id(const char *lexema) {
    if (strcasecmp(lexema, "program") == 0) return TOKEN_PROGRAM;
    if (strcasecmp(lexema, "var") == 0) return TOKEN_VAR;
    if (strcasecmp(lexema, "integer") == 0) return TOKEN_INTEGER;
    if (strcasecmp(lexema, "real") == 0) return TOKEN_REAL;
    if (strcasecmp(lexema, "begin") == 0) return TOKEN_BEGIN;
    if (strcasecmp(lexema, "end") == 0) return TOKEN_END;
    if (strcasecmp(lexema, "if") == 0) return TOKEN_IF;
    if (strcasecmp(lexema, "then") == 0) return TOKEN_THEN;
    if (strcasecmp(lexema, "else") == 0) return TOKEN_ELSE;
    if (strcasecmp(lexema, "while") == 0) return TOKEN_WHILE;
    if (strcasecmp(lexema, "do") == 0) return TOKEN_DO;
    return TOKEN_ID; 
}




int main(void){

    FILE * fp = fopen("leitura.pascal", "r");
    FILE * out = fopen("geracao.lex", "w");
    
    char Entrada[1024];
    int Linha = 1;

    while(fscanf(fp, "%[^\n]\n", Entrada) > 0) {
        Scanner S; 
        strcat(Entrada, "\n");
        printf("%s", Entrada);
        iniciar(&S, Entrada, Linha);

        for(;;){
            Token t = proximo_token(&S);
            fprintf(out, "(%s, %s) \t\t linha %d, col %d\n",
                nome_token(t.tipo), t.lexema, Linha, t.coluna);
            free(t.lexema);
            if(t.tipo==TOKEN_FIM || t.tipo==TOKEN_ERRO) break;
        }
        Linha++;
    }
}