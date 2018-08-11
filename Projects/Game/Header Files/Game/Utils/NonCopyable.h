#pragma once

class NonCopyable
{

public:

	NonCopyable () = default;

	virtual ~NonCopyable () = default;

	NonCopyable (NonCopyable&&) = delete;

	NonCopyable (const NonCopyable&) = delete;

	NonCopyable& operator=(NonCopyable&&) = delete;

	NonCopyable& operator=(const NonCopyable&) = delete;

};