# Simulador de Gerenciamento de Processos

## 📌 Descrição
Este projeto implementa um simulador de gerenciamento de processos em C, conforme especificação da disciplina de Sistemas Operacionais.

O simulador implementa:
- Criação de processos
- Troca de contexto
- Escalonamento
- Estados (pronto, bloqueado, execução)
- Substituição de imagem (exec)

## ⚙️ Tecnologias
- Linguagem C
- Linux
- System calls: fork, pipe, wait, sleep

## 📁 Estrutura
- `src/` → código fonte
- `data/` → programas simulados
- `tests/` → entradas de teste
- `docs/` → relatório

## ▶️ Como compilar

```bash
make