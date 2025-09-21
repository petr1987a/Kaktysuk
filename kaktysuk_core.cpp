#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <pybind11/pybind11.h>
#include "chess/chess.hpp"

namespace core {

const int piece_values[] = { 0, 100, 300, 300, 500, 900, 20000 };

int score_move(const chess::Board& board, const chess::Move& move) {
    int score = 0;
    // ФИНАЛЬНОЕ ИЗМЕНЕНИЕ: Правильное имя метода - promotionType()
    if (move.promotionType() != chess::PieceType::NONE) {
        score += 2000 + piece_values[static_cast<int>(move.promotionType())];
    }
    if (board.isCapture(move)) {
        auto victim_type = board.at<chess::Piece>(move.to()).type();
        auto attacker_type = board.at<chess::Piece>(move.from()).type();
        score += 1000 + (piece_values[static_cast<int>(victim_type)] * 10) - piece_values[static_cast<int>(attacker_type)];
    }
    
    chess::Board temp_board = board;
    temp_board.makeMove(move);
    if (temp_board.isAttacked(temp_board.kingSq(temp_board.sideToMove()), !temp_board.sideToMove())) {
        score += 500;
    }
    return score;
}

int evaluate(const chess::Board& board) {
    auto outcome = board.isGameOver();
    
    if (outcome.second != chess::GameResult::NONE) {
        if (outcome.second == chess::GameResult::WIN) return 99999;
        if (outcome.second == chess::GameResult::LOSE) return -99999;
        if (outcome.second == chess::GameResult::DRAW) return 0;
    }

    int score = 0;
    for (int i = 0; i < 64; ++i) {
        auto sq = chess::Square(i);
        auto piece = board.at<chess::Piece>(sq);
        if (piece.type() != chess::PieceType::NONE) {
            int value = piece_values[static_cast<int>(piece.type())];
            score += (piece.color() == chess::Color::WHITE) ? value : -value;
        }
    }
    return (board.sideToMove() == chess::Color::WHITE) ? score : -score;
}


int minimax(chess::Board& board, int depth, int alpha, int beta) {
    if (depth == 0 || board.isGameOver().second != chess::GameResult::NONE) {
        return evaluate(board);
    }

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    if (moves.empty()) {
        return evaluate(board);
    }

    std::sort(moves.begin(), moves.end(), [&](const chess::Move& a, const chess::Move& b) {
        return score_move(board, a) > score_move(board, b);
    });

    int best_score = -100000;
    for (const auto& move : moves) {
        board.makeMove(move);
        int score = -minimax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(move);

        if (score > best_score) {
            best_score = score;
        }
        if (best_score > alpha) {
            alpha = best_score;
        }
        if (alpha >= beta) {
            break; 
        }
    }
    return best_score;
}

std::string find_best_move_cpp(const std::string& fen, int depth) {
    chess::Board board(fen);
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    if (moves.empty()) {
        return "0000";
    }

    std::sort(moves.begin(), moves.end(), [&](const chess::Move& a, const chess::Move& b) {
        return score_move(board, a) > score_move(board, b);
    });

    chess::Move best_move = moves[0];
    int best_score = -100001;
    int alpha = -100001;
    int beta = 100001;

    for (const auto& move : moves) {
        board.makeMove(move);
        int score = -minimax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(move);
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    return chess::uci::moveToUci(best_move);
}

} 

PYBIND11_MODULE(kaktysuk_core, m) {
    m.doc() = "The C++ core for Kaktysuk-Zero engine";
    m.def("find_best_move", &core::find_best_move_cpp, "Finds the best move using C++ Minimax");
}