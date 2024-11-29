#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> // Biblioteca para configurar o locale

// Defini��o da estrutura Aluno
struct Aluno {
    int id;
    char nome[50];
    char curso[30];
    int ativo; // 1 para ativo, 0 para exclu�do
};

// Fun��es do programa
void menu();
void cadastrar();
void consultar();
void gerarRelatorio();
void excluir();
void atualizarArquivoTexto(); // Fun��o para atualizar o arquivo alunos.txt
int idAtivoExiste(int id);    // Fun��o para verificar duplica��o de IDs ativos

int main() {
    // Configura o locale para permitir acentos
    setlocale(LC_ALL, ""); // Configura��o para o idioma do sistema
    menu(); // Chama o menu principal
    return 0;
}

void menu() {
    int opcao;
    do {
        printf("\n=== Menu Principal ===\n");
        printf("1. Cadastro\n");
        printf("2. Consulta\n");
        printf("3. Gerar Relat�rio\n");
        printf("4. Excluir\n");
        printf("0. Sair\n");
        printf("Escolha uma op��o: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1: cadastrar(); break;
            case 2: consultar(); break;
            case 3: gerarRelatorio(); break;
            case 4: excluir(); break;
            case 0: printf("Saindo...\n"); break;
            default: printf("Op��o inv�lida!\n");
        }
    } while (opcao != 0);
}

void cadastrar() {
    FILE *binario = fopen("alunos.dat", "ab");
    if (!binario) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    struct Aluno aluno;
    printf("\nDigite o ID do aluno: ");
    scanf("%d", &aluno.id);

    if (idAtivoExiste(aluno.id)) {
        printf("Erro: J� existe um aluno ativo com esse ID!\n");
        fclose(binario);
        return;
    }

    getchar(); // Limpa o buffer
    printf("Digite o nome do aluno: ");
    fgets(aluno.nome, sizeof(aluno.nome), stdin);
    aluno.nome[strcspn(aluno.nome, "\n")] = '\0'; // Remove o '\n'
    printf("Digite o curso do aluno: ");
    fgets(aluno.curso, sizeof(aluno.curso), stdin);
    aluno.curso[strcspn(aluno.curso, "\n")] = '\0';
    aluno.ativo = 1;

    // Grava no arquivo bin�rio
    fwrite(&aluno, sizeof(struct Aluno), 1, binario);
    fclose(binario);

    // Atualiza o arquivo de texto
    atualizarArquivoTexto();

    printf("Aluno cadastrado com sucesso!\n");
}

void consultar() {
    FILE *arq = fopen("alunos.dat", "rb");
    if (!arq) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    int id;
    printf("\nDigite o ID do aluno a consultar: ");
    scanf("%d", &id);

    struct Aluno aluno;
    int encontrado = 0;

    while (fread(&aluno, sizeof(struct Aluno), 1, arq)) {
        if (aluno.id == id) {
            printf("\nAluno encontrado:\n");
            printf("ID: %d\nNome: %s\nCurso: %s\nAtivo: %d\n", aluno.id, aluno.nome, aluno.curso, aluno.ativo);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Aluno n�o encontrado!\n");
    }

    fclose(arq);
}

void gerarRelatorio() {
    FILE *arq = fopen("alunos.dat", "rb");
    if (!arq) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    struct Aluno aluno;
    printf("\n=== Relat�rio de Alunos ===\n");
    printf("\n--- Alunos Ativos ---\n");
    while (fread(&aluno, sizeof(struct Aluno), 1, arq)) {
        if (aluno.ativo) {
            printf("ID: %d | Nome: %s | Curso: %s\n", aluno.id, aluno.nome, aluno.curso);
        }
    }

    rewind(arq); // Volta ao in�cio do arquivo para listar os inativos
    printf("\n--- Alunos Desativados ---\n");
    while (fread(&aluno, sizeof(struct Aluno), 1, arq)) {
        if (!aluno.ativo) {
            printf("ID: %d | Nome: %s | Curso: %s\n", aluno.id, aluno.nome, aluno.curso);
        }
    }

    fclose(arq);
}

void excluir() {
    FILE *binario = fopen("alunos.dat", "r+b");
    if (!binario) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    int id;
    printf("\nDigite o ID do aluno a excluir: ");
    scanf("%d", &id);

    struct Aluno aluno;
    int encontrado = 0;

    while (fread(&aluno, sizeof(struct Aluno), 1, binario)) {
        if (aluno.id == id && aluno.ativo) {
            aluno.ativo = 0; // Marca como inativo
            fseek(binario, -sizeof(struct Aluno), SEEK_CUR); // Reescreve no arquivo bin�rio
            fwrite(&aluno, sizeof(struct Aluno), 1, binario);

            printf("Aluno exclu�do com sucesso!\n");
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Aluno n�o encontrado ou j� exclu�do!\n");
    }

    fclose(binario);

    // Atualiza o arquivo de texto
    atualizarArquivoTexto();
}

// Fun��o para verificar se j� existe um aluno ativo com o mesmo ID
int idAtivoExiste(int id) {
    FILE *arq = fopen("alunos.dat", "rb");
    if (!arq) return 0;

    struct Aluno aluno;
    while (fread(&aluno, sizeof(struct Aluno), 1, arq)) {
        if (aluno.id == id && aluno.ativo) {
            fclose(arq);
            return 1; // ID encontrado entre os ativos
        }
    }

    fclose(arq);
    return 0; // N�o encontrado
}

// Fun��o para atualizar o arquivo alunos.txt com divis�es entre ativos e desativados
void atualizarArquivoTexto() {
    FILE *binario = fopen("alunos.dat", "rb");
    FILE *texto = fopen("alunos.txt", "w");
    if (!binario || !texto) {
        printf("Erro ao atualizar o arquivo de texto!\n");
        return;
    }

    struct Aluno aluno;

    // Escreve os alunos ativos
    fprintf(texto, "\n=================================================");
    fprintf(texto, "Ativos:\n");
    while (fread(&aluno, sizeof(struct Aluno), 1, binario)) {
        if (aluno.ativo) {
            fprintf(texto, "ID: %d | Nome: %s | Curso: %s | Ativo: %d\n", aluno.id, aluno.nome, aluno.curso, aluno.ativo);
        }
    }

    // Escreve os alunos desativados
    rewind(binario);
    fprintf(texto, "\nDesativados:\n");
    fprintf(texto, "=================================================\n");
    while (fread(&aluno, sizeof(struct Aluno), 1, binario)) {
        if (!aluno.ativo) {
            fprintf(texto, "ID: %d | Nome: %s | Curso: %s | Ativo: %d\n", aluno.id, aluno.nome, aluno.curso, aluno.ativo);
        }
    }

    fclose(binario);
    fclose(texto);
}

