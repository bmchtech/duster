require 'ldusk'
require 'lduster'
require 'game_ai'

global function game_gs_ai_plan_moves_variant_1(team_id: cint, move_buf: *QueuedMove, move_buf_len: cint): cint <cexport, codename 'game_gs_ai_plan_moves_variant_1'>
    return ai_plan_moves_variant_1(team_id, move_buf, move_buf_len)
end

global function game_gs_ai_plan_moves_variant_2(team_id: cint, move_buf: *QueuedMove, move_buf_len: cint): cint <cexport, codename 'game_gs_ai_plan_moves_variant_2'>
    return ai_plan_moves_variant_2(team_id, move_buf, move_buf_len)
end