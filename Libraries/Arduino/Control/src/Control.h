#pragma once
#include <Graphics.h>
#include <stdint.h>
#include <stddef.h>


namespace graphics
{
	class Control
	{
		public:
		Control()
		{
		}
		
		Control(const uint8_t id,
		        const Size* clipSize, 
		        const Rect rect,
		        void(*paintControl)(Control*, Graphics*),
		        const Size* quantiseSizeBitShift = &ZERO_BITSHIFT,
		        const uint8_t zOrder = 0)
			: ID_(id)
			, CLIP_SIZE_(clipSize)
			, RECT_(rect)
			, QUANTISE_SIZE_BITSHIFT_(quantiseSizeBitShift)
			, paintControl_(paintControl)
			, Z_ORDER_(zOrder)
		{
		}
	
		~Control(void)
		{
		}
		
		inline void paintAll()
		{
			changedRect_.location = {};
			changedRect_.size = RECT_.size;
			paintChanged();
		}
		
		inline void paintChanged()
		{
			if (changedRect_.size.width == 0 || changedRect_.size.height == 0)
			{
				return;
			}			
			
			graphics::clip(&changedRect_, CLIP_SIZE_, QUANTISE_SIZE_BITSHIFT_);
			
			Graphics graphics( { 
			                  {
				                  RECT_.location.x + changedRect_.location.x, 
				                  RECT_.location.y + changedRect_.location.y
			                  }, 
				                  changedRect_.size
			                  }, 
			                  CLIP_SIZE_);
	
			paintGraphics(&graphics);
			paintControl_(this, &graphics);	
			
			changedRect_ = {};
		}
		
		inline const uint8_t getId() const 
		{
			return ID_;
		}
	
		protected:
		inline const Rect* getRectPtr() const
		{
			return &RECT_;
		}
		
		inline void setChangedAll()
		{
			changedRect_ = 
			{ 
				{ 0, 0 },
				RECT_.size
			};	
		}
		
		inline void addChanged(Rect* addRect)
		{
			enlarge(&changedRect_, addRect);
		}
		
		inline void addChanged(Point* addPoint)
		{
			enlarge(&changedRect_, addPoint);
		}
		
		inline void addChanged(Grid* addGrid)
		{
			enlarge(&changedRect_, addGrid, QUANTISE_SIZE_BITSHIFT_);
		}
		
		inline const Rect* getChangedPtr() const
		{
			return &changedRect_;
		}
		
		inline const GridSize getGridSize() const
		{
			return
			{ 
				RECT_.size.width >> QUANTISE_SIZE_BITSHIFT_->width,
				RECT_.size.height >> QUANTISE_SIZE_BITSHIFT_->height
			};
		}
		
		inline const Grid getChangedGrid() const
		{
			return 
			{ 
			{
				changedRect_.location.x >> QUANTISE_SIZE_BITSHIFT_->width,
				changedRect_.location.y >> QUANTISE_SIZE_BITSHIFT_->height
			},
			{
				changedRect_.size.width >> QUANTISE_SIZE_BITSHIFT_->width,
				changedRect_.size.height >> QUANTISE_SIZE_BITSHIFT_->height
			}
			};
		}
		
		inline const Rect getControlRect() const
		{
			return
			{ 
				{ 0, 0 },
				RECT_.size
			};
		}
		
		inline bool isAllChanged()
		{
			const Rect CONTROL_RECT = getControlRect();
			return isEqual(&changedRect_, &CONTROL_RECT);
		}
		
		virtual void paintGraphics(Graphics* graphics) = 0;
		
		private:
		void(*paintControl_)(Control*, Graphics*) = NULL;
		const Size* QUANTISE_SIZE_BITSHIFT_ = &ZERO_BITSHIFT;
		const Size* CLIP_SIZE_ = NULL;
		const uint8_t ID_ = 0;
		const uint8_t Z_ORDER_ = 0;
		const Rect RECT_ = { };
		static constexpr Size ZERO_BITSHIFT = { 0, 0 };
		Rect changedRect_ = {};
	};
}
