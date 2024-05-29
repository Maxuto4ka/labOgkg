#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace sf;
using namespace std;


struct Point {
	int x;
	int y;
};

const int W = 852;
const int H = 852;

// Функція для генерації випадкових точок
vector<Point> generateRandomPoints(int numPoints, int minVal, int maxVal) {
	vector<Point> points;
	srand(time(0)); 

	for (int i = 0; i < numPoints; ++i) {
		Point p;
		p.x = minVal + rand() % (maxVal - minVal + 1);
		p.y = minVal + rand() % (maxVal - minVal + 1);
		points.push_back(p);
	}

	return points;
}

// Функція для сортування точок за координатами x, y
void sortPoints(vector<Point>& points) {
	sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
		if (a.x == b.x) {
			return a.y < b.y;
		}
		return a.x < b.x;
		});
}

// Функція для визначення орієнтації трьох точок (a, b, c)
// Повертає:
// 0, якщо точки колінеарні
// 1, якщо орієнтація за годинниковою стрілкою
// 2, якщо орієнтація проти годинникової стрілки
int orientation(const Point& a, const Point& b, const Point& c) {
	int val = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
	if (val == 0) return 0;
	return (val > 0) ? 1 : 2;
}

// Функція для знаходження опуклої оболонки методом Джарвіса
vector<Point> convexHullJarvis(const vector<Point>& points) {
	if (points.size() < 3) return {}; // Потрібно принаймні 3 точки

	vector<Point> hull;

	int leftmost = 0;
	
	int p = leftmost, q;
	do {
		hull.push_back(points[p]);
		q = (p + 1) % points.size();
		for (int i = 0; i < points.size(); i++) {
			if (orientation(points[p], points[i], points[q]) == 2) {
				q = i;
			}
		}
		p = q;
	} while (p != leftmost);

	return hull;
}

// Функція для знаходження площі трикутника за координатами його вершин
double triangleArea(const Point & p1, const Point & p2, const Point & p3) {
	return std::abs((p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0);
}


// Функція для знаходження найбільшого за площею трикутника, вписаного в опуклу оболонку
vector<Point> largestInscribedTriangle(const vector<Point>& convexHull) {
	int n = convexHull.size();
	if (n < 3) return {};

	double maxArea = 0.0;
	vector<Point> maxTriangle;

	auto findMaxTriangle = [&](const vector<Point>& points, int left, int right) {
		if (right - left < 2) return;

		// Шукаємо всі можливі трикутники у відрізку між left і right
		for (int i = left; i <= right - 2; ++i) {
			for (int j = i + 1; j <= right - 1; ++j) {
				for (int k = j + 1; k <= right; ++k) {
					// Обчислюємо площу трикутника, який має вибрані три точки як вершини
					double area = triangleArea(points[i], points[j], points[k]);
					// Оновлюємо максимальну площу, якщо знайдено більшу
					if (area > maxArea) {
						maxArea = area;
						maxTriangle = { points[i], points[j], points[k] };
					}
				}
			}
		}
		};

	// Викликаємо функцію для знаходження трикутника в опуклій оболонці
	findMaxTriangle(convexHull, 0, n - 1);

	return maxTriangle;
}




int main()
{
	const int numPoints = 20;
	const int minVal = -400;
	const int maxVal = 400;
	RenderWindow window(VideoMode(W, H), "LabaOGKG");

	int x0 = W / 2;
	int y0 = H / 2;

	vector<Point> points = generateRandomPoints(numPoints, minVal, maxVal);
	sortPoints(points);
	vector<Point> hull = convexHullJarvis(points);
	vector<Point> maxTriangle = largestInscribedTriangle(hull);


	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		window.clear(Color::White);

		for (const Point& p : points) {
			CircleShape shape(5);
			shape.setFillColor(Color::Blue);
			shape.setPosition(p.x + x0, y0 - p.y); 
			window.draw(shape);
		}

		if (!hull.empty()) {
			VertexArray lines(LineStrip, hull.size() + 1);
			for (int i = 0; i < hull.size(); ++i) {
				lines[i].position = Vector2f(hull[i].x + x0 + 5, y0 - hull[i].y + 5);
				lines[i].color = Color::Green;
			}
			lines[hull.size()].position = Vector2f(hull[0].x + x0 + 5, y0 - hull[0].y + 5);
			lines[hull.size()].color = Color::Green;
			window.draw(lines);
		}

		if (maxTriangle.size() == 3) {
			VertexArray triangleLines(LinesStrip, 4);
			for (int i = 0; i < 3; ++i) {
				triangleLines[i].position = Vector2f(maxTriangle[i].x + x0 + 5, y0 - maxTriangle[i].y + 5);
				triangleLines[i].color = Color::Red;
			}
			triangleLines[3].position = Vector2f(maxTriangle[0].x + x0 + 5, y0 - maxTriangle[0].y + 5);
			triangleLines[3].color = Color::Red;
			window.draw(triangleLines);
		}

		window.display();
	}


	return 0;
}
