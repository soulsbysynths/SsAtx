#pragma once
#include <Graphics.h>
#include <stdint.h>
#include <stddef.h>


namespace graphics
{
	class Control
	{
		public:
		Control(const uint8_t id,
		        const Size* constrainSize, 
		        const Rect rect,
		        void(*paintControl)(Control*, Graphics*),
		        const uint8_t zOrder = 0)
			: ID_(id)
			, CONSTRAIN_SIZE_(constrainSize)
			, RECT_(rect)
			, paintControl_(paintControl)
			, Z_ORDER_(zOrder)
		{
		}
	
		~Control(void)
		{
		}
	
		inline void paint()
		{
			if (paintRect_.size.width == 0 || paintRect_.size.height == 0)
			{
				return;
			}
		
			paint(&paintRect_);
			
			paintRect_ = { };
		}
	
		inline const uint8_t getId() const 
		{
			return ID_;
		}
	
		protected:
		inline virtual void paint(Rect* rect)
		{
			Graphics graphics( { 
			                  {
				                  RECT_.location.x + rect->location.x, 
				                  RECT_.location.y + rect->location.y
			                  }, 
				                  rect->size
			                  }, 
			                  CONSTRAIN_SIZE_);
	
			paintGraphics(rect, &graphics);
			paintControl_(this, &graphics);	
		}
		virtual void paintGraphics(Rect* rect, Graphics* graphics) = 0;
		void(*paintControl_)(Control*, Graphics*) = NULL;
		Rect paintRect_ = { };
		const Size* CONSTRAIN_SIZE_;
		const Rect RECT_;
		const uint8_t ID_;
		const uint8_t Z_ORDER_;
	};
}
