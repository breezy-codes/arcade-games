#include "splashkit.h"
#include "game.h"
#include "debugging.h"

void load_resources()
{
    load_resource_bundle("game_bundle", "icons.txt");
    load_music("game", "game.wav");
    load_music("intro", "intro.mp3");
    open_audio();
}

int main()
{
    const string WINDOW_NAME = "VentureAdventure";
    const int WINDOW_WIDTH = 672;
    const int WINDOW_HEIGHT = SCREEN_HEIGHT;

    open_window(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT);
    window_toggle_border(WINDOW_NAME);
    load_resources();

    const int TOTAL_LEVELS = 2;     // Remember to update this value when adding new levels
    const float MUSIC_VOLUME_INTRO = 0.05;
    const float MUSIC_VOLUME_LOOP = 0.03;

    string level_map = "Resources/levels/level1.txt";   // Use this to adjust starting level (e.g., for debugging)
    int level_id = 1;
    bool level_completed;

    game_data game;
    game = new_game(level_map);

    bool debugging_output_enabled = true;      // Change this to toggle the debugging output on/off
    vector<string> old_debug_message = { "" };
    bool still_waiting = false;

    while (!quit_requested() && !key_down(ESCAPE_KEY))
    {
        play_music("intro");
        set_music_volume(MUSIC_VOLUME_INTRO);

        start_screen();
        fade_music_out(1000);

        while (!quit_requested() && !key_down(ESCAPE_KEY))
        {
            process_events();
            clear_screen();
            draw_game(game);

            if (!music_playing() && !sound_effect_playing("level_win"))
            {
                play_music("game", 100);
                set_music_volume(MUSIC_VOLUME_LOOP);
            }

            level_completed = update_game(game, level_id);

            if (game.player.attacked == true)
            {
                double text_x = SCREEN_WIDTH / 2 - 138;
                double text_y = SCREEN_HEIGHT / 2 - 48;
                int font_size = 70;

                draw_text("Game Over", COLOR_BLANCHED_ALMOND, "font.ttf", font_size, text_x, text_y, option_to_screen());
            }

            if (debugging_output_enabled)
            {
                process_debugging(game, old_debug_message, still_waiting);
            }

            // Intended for use when soft-locked/trapped by boxes without needing to restart game. Will reset gems and player on current level, but won't reset lives
            if (key_down(R_KEY))
            {
                game = new_game(level_map);
            }

            refresh_screen(60);

            if (level_completed && level_id <= TOTAL_LEVELS)
            {
                level_id++;
                level_map = "Resources/levels/level" + std::to_string(level_id) + ".txt";
                game = new_game(level_map);
            };

            if (level_completed && level_id > TOTAL_LEVELS)
            {
                delay(5000);

                play_music("intro");
                set_music_volume(MUSIC_VOLUME_INTRO);

                start_screen();
                fade_music_out(1000);

                level_id = 1;
                level_map = "Resources/levels/level" + std::to_string(level_id) + ".txt";
                game = new_game(level_map);
            };
        }
    }

    return 0;
}
