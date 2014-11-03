#pragma once

namespace Common
{
    class NonCopyable
    {
		public:
			NonCopyable() {}
		  
		private:

            NonCopyable( const NonCopyable& /*that*/ ) {}
            NonCopyable& operator=( const NonCopyable& /*that*/ ) { return *this; }
    };
} // Common 
