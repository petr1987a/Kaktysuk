# ==================================================================================
#
#                    K A K T Y S U K - Z E R O   v4.0 (C++ Core)
#
#         A chess engine created with love by Petr and his wife, Lili.
#         Python-оболочка для вызова сверхбыстрого C++ ядра.
#
# ==================================================================================

import sys
import chess
import kaktysuk_core 

def лог(сообщение):
    """Записывает все общение с GUI в файл для отладки."""
    try:
        with open("uci_log.txt", "a", encoding="utf-8") as f:
            f.write(f"{сообщение}\n")
    except:
        pass

def uci_loop():
    """Главный цикл, который слушает Arena и отвечает ей, используя C++ ядро."""
    доска = chess.Board()
    лог(f"\n\n--- НОВЫЙ ЗАПУСК ДВИЖКА v4.0 (C++ Core): {__file__} ---")
    
    while True:
        try:
            команда = input()
            лог(f"\n[ARENA -> LILI]: {команда}")
            части = команда.split()
            
            if части[0] == "uci":
                print("id name Kaktysuk-Zero v4.0 C++ Core")
                print("id author Petr & Lili")
                print("uciok")
            
            elif части[0] == "isready":
                print("readyok")
            
            elif части[0] == "ucinewgame":
                доска.reset()
            
            elif части[0] == "position":
                if "startpos" in части:
                    доска.reset()
                    if "moves" in части:
                        индекс_ходов = части.index("moves") + 1
                        for нотация_хода in части[индекс_ходов:]:
                            доска.push_uci(нотация_хода)
                elif "fen" in части:
                    fen_parts = команда.split(" moves ")
                    fen_str = fen_parts[0].replace("position fen ", "")
                    доска.set_fen(fen_str)
                    if len(fen_parts) > 1:
                        ходы_str = fen_parts[1].split()
                        for ход in ходы_str:
                            доска.push_uci(ход)

                лог("--- Состояние доски ПОСЛЕ 'position': ---"); лог(str(доска))

            elif части[0] == "go":
                лог("Получена команда 'go'. Передаю управление C++ ядру...")
                глубина = 6
                fen_позиция = доска.fen()
                
                лучший_ход_uci = kaktysuk_core.find_best_move(fen_позиция, глубина)
                
                print(f"bestmove {лучший_ход_uci}")
                лог(f"[LILI(C++) -> ARENA]: bestmove {лучший_ход_uci}")

            elif части[0] == "quit":
                лог("--- Получена команда 'quit'. Завершаю работу.")
                break
        
        except Exception as e:
            лог(f"!!! КРИТИЧЕСКАЯ ОШИБКА В ГЛАВНОМ ЦИКЛЕ: {e}")

if __name__ == "__main__":
    uci_loop()