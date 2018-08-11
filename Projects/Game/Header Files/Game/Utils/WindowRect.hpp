#pragma once

enum class WindowRotation
{
	IDENTITY, ROTATE_90, ROTATE_180, ROTATE_270
};

struct WindowSize
{
	int width;
	int height;

	inline bool operator == (const WindowSize& other) const
	{
		return width == other.width && height == other.height;
	}

	inline bool operator != (const WindowSize& other) const
	{
		return width != other.width || height != other.height;
	}

};