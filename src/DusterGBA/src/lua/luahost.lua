require 'ldusk'
require 'lduster'
require 'game_ai'

global function game_gs_ai_plan_moves(team_id: cint, move_buf: *QueuedMove, move_buf_len: cint): cint <cexport, codename 'game_gs_ai_plan_moves'>
    return ai_plan_moves(team_id, move_buf, move_buf_len)
end