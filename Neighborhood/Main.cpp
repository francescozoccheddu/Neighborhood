#include "Window.hpp"

int main ()
{
	Window w;
	w.Create ();
	w.SetTitle ("Prova");
	w.SetVisible (true);
	w.SetSystemButtons (true, false, false);
	w.minH = w.minW = 100;
	w.maxW = w.maxH = 200;
	w.Run ();
}