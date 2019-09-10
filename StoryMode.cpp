#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "MenuMode.hpp"

Sprite const *sprite_bg = nullptr;
Sprite const *sprite_player = nullptr;
Sprite const *sprite_leaf_player = nullptr;

Sprite const *sprite_lollys_lolly = nullptr;
Sprite const *sprite_lollys_house = nullptr;
Sprite const *sprite_lollys_leaf = nullptr;

Sprite const *text_lolly_request = nullptr;
Sprite const *text_lolly_no_item = nullptr;
Sprite const *text_lolly_give_item = nullptr;
Sprite const *text_lolly_thanks = nullptr;
Sprite const *text_lolly_goto = nullptr;

Sprite const *sprite_goldies_goldie = nullptr;
Sprite const *sprite_goldies_house = nullptr;

Sprite const *text_goldie_greeting = nullptr;
Sprite const *text_goldie_request_item = nullptr;
Sprite const *text_goldie_give_item = nullptr;
Sprite const *text_goldie_goto = nullptr;

Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("ac"));

	sprite_bg = &ret->lookup("background");
	sprite_player = &ret->lookup("player");
    sprite_leaf_player = &ret->lookup("leaf_player");

	sprite_lollys_lolly = &ret->lookup("lolly");
    sprite_lollys_house = &ret->lookup("lolly-house");
    sprite_lollys_leaf = &ret->lookup("leaf_lolly");
    text_lolly_request = &ret->lookup("text-request");
    text_lolly_no_item = &ret->lookup("text-no-hammock");
    text_lolly_give_item = &ret->lookup("text-give-hammock");
    text_lolly_thanks = &ret->lookup("text-thanks");
    text_lolly_goto = &ret->lookup("text-goto-goldies");

	sprite_goldies_goldie = &ret->lookup("goldie");
	sprite_goldies_house = &ret->lookup("goldie-house");
    text_goldie_greeting = &ret->lookup("text-greeting");
    text_goldie_request_item = &ret->lookup("text-request-item");
    text_goldie_give_item = &ret->lookup("text-give-leaf");
    text_goldie_goto = &ret->lookup("text-goto-lolly");

	return ret;
});

StoryMode::StoryMode() {
}

StoryMode::~StoryMode() {
}

bool StoryMode::handle_event(SDL_Event const &, glm::uvec2 const &window_size) {
	if (Mode::current.get() != this) return false;

	return false;
}

void StoryMode::update(float elapsed) {
	if (Mode::current.get() == this) {
		//there is no menu displayed! Make one:
		enter_scene();
	}
}

void StoryMode::enter_scene() {
	//just entered this scene, adjust flags and build menu as appropriate:
	std::vector< MenuMode::Item > items;
	glm::vec2 at(3.0f, view_max.y - 3.0f);
	//auto add_text = [&items,&at](Sprite const *text) {
		//assert(text);
		//items.emplace_back("TEST TEXT", nullptr, 1.0f, nullptr, at);
		//at.y -= text->max_px.y - text->min_px.y;
		//at.y -= 4.0f;
	//};
	//auto add_choice = [&items,&at](Sprite const *text, std::function< void(MenuMode::Item const &) > const &fn) {
		//assert(text);
		//items.emplace_back(text, nullptr, 1.0f, fn, at + glm::vec2(8.0f, 0.0f));
		//at.y -= text->max_px.y - text->min_px.y;
		//at.y -= 4.0f;
	//};

	if (location == Lolly) {
		if (lollys.gave_leaf) {
			lollys.gave_leaf = false;
		}
		if (lollys.first_visit) {
			lollys.first_visit = false;
		} else {
		}
		at.y -= 8.0f; //gap before choices
	} else if (location == Goldie) {
		if (goldies.took_leaf) {
			goldies.took_leaf = false;
		}
		if (goldies.first_visit) {
			goldies.first_visit = false;
		} else {
		}
		at.y -= 8.0f; //gap before choices
	}
	std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >(items);
	menu->atlas = sprites;
	//menu->left_select = sprite_left_select;
	//menu->right_select = sprite_right_select;
	menu->view_min = view_min;
	menu->view_max = view_max;
	menu->background = shared_from_this();
	Mode::current = menu;
}

void StoryMode::draw(glm::uvec2 const &drawable_size) {
	//clear the color buffer:
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //use a DrawSprites to do the drawing:
		DrawSprites draw(*sprites, view_min, view_max, drawable_size, DrawSprites::AlignPixelPerfect);
		glm::vec2 ul = glm::vec2(view_min.x, view_max.y);
		if (location == Lolly) {
			draw.draw(*sprite_bg, ul);
			draw.draw(*sprite_lollys_house, ul);
			draw.draw(*sprite_lollys_lolly, ul);
            draw.draw(*sprite_player, ul);
            draw.draw(*text_lolly_give_item, ul);
            draw.draw(*text_lolly_goto, ul);
            if (has_leaf) {
                draw.draw(*sprite_leaf_player, ul);
            }
            if (lollys.gave_leaf) {
                draw.draw(*text_lolly_thanks, ul);
                draw.draw(*sprite_lollys_leaf, ul);
            } else {
                draw.draw(*text_lolly_request, ul);
            }
		} else if (location == Goldie) {
			draw.draw(*sprite_bg, ul);
			draw.draw(*sprite_goldies_house, ul);
            draw.draw(*sprite_goldies_goldie, ul);
            draw.draw(*sprite_player, ul);
            draw.draw(*text_goldie_goto, ul);
            if (has_leaf) {
                draw.draw(*sprite_leaf_player, ul);
            }
            if (goldies.took_leaf) {
                draw.draw(*text_goldie_give_item, ul);
            } else {
                draw.draw(*text_goldie_greeting, ul);
                draw.draw(*text_goldie_request_item, ul);
            }
        }
	}
	GL_ERRORS(); //did the DrawSprites do something wrong?
}
