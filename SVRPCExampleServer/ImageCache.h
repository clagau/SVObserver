#pragma once

#include <map>
#include <string>
#include <sstream>
#include <vector>

namespace SVRPCExample
{
struct ImageCache
{
	std::map<unsigned int, std::vector<std::string>> m_cache;
	const int m_num {3};

	void getImage(std::string& data, uint32_t size, size_t idx)
	{
		auto& images = m_cache[size];
		if (images.empty())
		{
			images.resize(m_num);
			for (int i = 0; i < m_num; ++i)
			{
				int red_mult = i % 3 > 0 ? 1 : 0;
				int green_mult = (i + 1) % 3 > 0 ? 1 : 0;
				int blue_mult = (i + 2) % 3 > 0 ? 1 : 0;
				createImageOfSize(images[i], size, size, red_mult, green_mult, blue_mult);
			}
		}
		data = images[idx % m_num];
	}

	void createImageOfSize(std::string& data, unsigned int width, unsigned int height, int red_mult, int green_mult, int blue_mult)
	{
		bitmap_image image(width, height);
		image.clear();

		for (unsigned int y = 0; y < height; ++y)
		{
			for (unsigned int x = 0; x < width; ++x)
			{
				const unsigned char red = red_mult * std::round(255 * x / (1.0 * width));
				const unsigned char green = green_mult * std::round(255 * x / (1.0 * width));
				const unsigned char blue = blue_mult * std::round(255 * x / (1.0 * width));
				image.set_pixel(x, y, red, green, blue);
			}
		}

		std::ostringstream ss;
		image.save_image(ss);
		data = ss.str();
	}
};

}
