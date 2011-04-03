/******************************
** Tsunagari Tile Engine     **
** window.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "resourcer.h"
#include "window.h"

GameWindow::GameWindow() : Gosu::Window(640, 480, false)
{
	Resourcer rc(this);
	world = new World(this, &rc);
}

GameWindow::~GameWindow()
{
	delete world;
}

void GameWindow::update()
{
}

void GameWindow::draw()
{
	world->draw();
}

bool GameWindow::needsRedraw()
{
	return world->needs_redraw();
}

