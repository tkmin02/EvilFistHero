#include <random>
#include <numbers>
#include "tnl_math.h"

namespace tnl {

    std::vector<double> Solve2(std::vector<double>& c) {
        double p = c[1] / (2 * c[2]);
        double q = c[0] / c[2];
        double D = p * p - q;
        if (IsZeroD(D)) {
            return {-p};
        }else if (D < 0) {
            return {};
        }else /* if (D > 0) */ {
            double sqrt_D = sqrt(D);
            return { sqrt_D - p, -sqrt_D - p };
        }
    }
    std::vector<double> Solve3(std::vector<double>& c) {
        /* normal form: x^3 + Ax^2 + Bx + C = 0 */
        double A = c[2] / c[3];
        double B = c[1] / c[3];
        double C = c[0] / c[3];

        /*  substitute x = y - A/3 to eliminate quadric term:
        x^3 +px + q = 0 */
        double sq_A = A * A;
        double p = 1.0 / 3 * (-1.0 / 3 * sq_A + B);
        double q = 1.0 / 2 * (2.0 / 27 * A * sq_A - 1.0 / 3 * A * B + C);

        /* use Cardano's formula */

        double cb_p = p * p * p;
        double D = q * q + cb_p;

        std::vector<double> s ;

        if (IsZeroD(D)) {
            if (IsZeroD(q)) /* one triple solution */ {
                s = {0};
            }else /* one single and one double solution */ {
                double u = cbrt(-q);
                s = { 2 * u, -u };
            }
        }else if (D < 0) /* Casus irreducibilis: three real solutions */ {
            double phi = 1.0 / 3 * acos(-q / sqrt(-cb_p));
            double _stage_id = 2 * sqrt(-p);
            s = { _stage_id * cos(phi),
                -_stage_id * cos(phi + std::numbers::pi / 3),
                -_stage_id * cos(phi - std::numbers::pi / 3) };

        }
        else /* one real solution */ {
            double sqrt_D = sqrt(D);
            double u = cbrt(sqrt_D - q);
            double v = -cbrt(sqrt_D + q);
            s = { u + v };
        }

        /* resubstitute */
        double sub = 1.0 / 3 * A;
        for (uint32_t i = 0; i < s.size(); ++i)
            s[i] -= sub;
        return s;
    }
    std::vector<double> Solve4(std::vector<double>& c) {
        /* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */
        double A = c[3] / c[4];
        double B = c[2] / c[4];
        double C = c[1] / c[4];
        double D = c[0] / c[4];

        /*  substitute x = y - A/4 to eliminate cubic term:
        x^4 + px^2 + qx + r = 0 */
        double sq_A = A * A;
        double p = -3.0 / 8 * sq_A + B;
        double q = 1.0 / 8 * sq_A * A - 1.0 / 2 * A * B + C;
        double r = -3.0 / 256 * sq_A * sq_A + 1.0 / 16 * sq_A * B - 1.0 / 4 * A * C + D;
        std::vector<double> s ;

        if (IsZeroD(r)) {
            /* no absolute term: y(y^3 + py + q) = 0 */
            std::vector<double> coeffs = { q, p, 0, 1 };
            s = Solve3(coeffs);
            s.emplace_back(0);
        }
        else {
            /* solve the resolvent cubic ... */
            std::vector<double> coeffs = {
                1.0 / 2 * r * p - 1.0 / 8 * q * q,
                    -r,
                    -1.0 / 2 * p,
                    1 };
            s = Solve3(coeffs);

            /* ... and take the one real solution ... */
            double z = s[0];

            /* ... to build two quadric equations */
            double u = z * z - r;
            double v = 2 * z - p;

            if (IsZeroD(u))
                u = 0;
            else if (u > 0)
                u = sqrt(u);
            else
                return {};

            if (IsZeroD(v))
                v = 0;
            else if (v > 0)
                v = sqrt(v);
            else
                return {};

            coeffs = {
                z - u,
                    q < 0 ? -v : v,
                    1 };

            s = Solve2(coeffs);

            coeffs = { z + u,
                q < 0 ? v : -v,
                1 };
            auto _stage_id = Solve2(coeffs);
            s.insert(s.end(), _stage_id.begin(), _stage_id.end());
        }

        /* resubstitute */
        double sub = 1.0 / 4 * A;
        for (uint32_t i = 0; i < s.size(); ++i)
            s[i] -= sub;

        return s;
    }


    std::mt19937 g_mersenne ;
    void SetSeedMersenneTwister32(int seed) {
        g_mersenne.seed(seed);
    }

    float GetRandomDistributionFloat(float min, float max) {
        std::uniform_real_distribution<> param(static_cast<double>(min), static_cast<double>(max));
        return static_cast<float>(param(g_mersenne));
    }


	int GetSidesPointAndPlane(const Vector3& v, const Vector3& pn, const Vector3& pv)
	{
		Vector3 vec = v - pv;
		float a = vec.dot( pn );
		if (a > 0.0f) 		return 1;		// 表
		else if (a < 0.0f)	return -1;		// 裏
		else 				return 0;		// 平面上
	}


	int GetRegionPointAndRect(const Vector3& p, const Vector3& rp, const int rect_w, const int rect_h) {
		tnl::Vector3 v1 = tnl::Vector3::Normalize({ float(rect_h), float(rect_w), 0 });
		tnl::Vector3 v2 = tnl::Vector3::Normalize({ float(rect_h), float(-rect_w), 0 });
//		tnl::Vector3 vc1 = v1.cross(tnl::Vector3(0, 0, 1));
//		tnl::Vector3 vc2 = v2.cross(tnl::Vector3(0, 0, 1));
		tnl::Vector3 vc1 = v1;
		tnl::Vector3 vc2 = v2;

		int s1 = tnl::GetSidesPointAndPlane(p, vc1, rp);
		int s2 = tnl::GetSidesPointAndPlane(p, vc2, rp);
		if (s1 >= 0 && s2 >= 0) {
			return 1;
		}
		else if (s1 >= 0 && s2 <= 0) {
			return 2;
		}
		else if (s1 <= 0 && s2 >= 0) {
			return 0;
		}
		else {
			return 3;
		}
	}

    int GetRegionPointAndAABB(const Vector3& p, const Vector3& max, const Vector3& min) {
        int side[4] = { 0, 0, 0, 0 };
        tnl::Vector3 t_max[4];
        tnl::Vector3 t_min[4];
        tnl::Vector3 vn[4];
        t_max[0] = { max.x, 0, max.z };
        t_min[0] = { min.x, 0, min.z };
        t_max[1] = { max.x, 0, -max.z };
        t_min[1] = { min.x, 0, -min.z };
        t_max[2] = { 0, max.y, max.z };
        t_min[2] = { 0, min.y, min.z };
        t_max[3] = { 0, -max.y, max.z };
        t_min[3] = { 0, -min.y, min.z };
        vn[0] = tnl::Vector3::Normalize(tnl::Vector3::Cross((t_min[0] - t_max[0]), { 0, -1, 0 }));
        vn[1] = tnl::Vector3::Normalize(tnl::Vector3::Cross((t_min[1] - t_max[1]), { 0, -1, 0 }));
        vn[2] = tnl::Vector3::Normalize(tnl::Vector3::Cross((t_min[2] - t_max[2]), { -1, 0, 0 }));
        vn[3] = tnl::Vector3::Normalize(tnl::Vector3::Cross((t_min[3] - t_max[3]), { -1, 0, 0 }));

        for (int i = 0; i < 4; ++i) side[i] = GetSidesPointAndPlane(p, vn[i], t_max[i]);

        // 前の領域に存在
        if (-1 == side[0] && -1 == side[1] && 1 == side[2] && -1 == side[3]) return 4;

        // 後の領域に存在
        if (1 == side[0] && 1 == side[1] && -1 == side[2] && 1 == side[3]) return 5;

        return GetRegionPointAndRect(p, (max + min) * 0.5f, (int)(max.x - min.x), (int)(max.y - min.y));
    }



    int GetXzRegionPointAndOBB(const Vector3& p, const Vector3& op, const Vector3& size, const Quaternion& q) {
        tnl::Vector3 pv1 = tnl::Vector3::Normalize(size) * 0.5f ;
        tnl::Vector3 pv2 = {-pv1.x, pv1.y, pv1.z};
        tnl::Vector3 pn1 = tnl::Vector3::Normalize( tnl::Vector3::Cross(pv1, pv1.xz()) );
        tnl::Vector3 pn2 = tnl::Vector3::Normalize( tnl::Vector3::Cross(pv2, pv2.xz()) );
        pv1 = op + tnl::Vector3::TransformCoord(pv1, q);
        pv2 = op + tnl::Vector3::TransformCoord(pv2, q);
        pn1 = tnl::Vector3::TransformCoord(pn1, q);
        pn2 = tnl::Vector3::TransformCoord(pn2, q);

        int s1 = tnl::GetSidesPointAndPlane(p, pn1, pv1);
        int s2 = tnl::GetSidesPointAndPlane(p, pn2, pv2);
        if (s1 >= 0 && s2 >= 0) {
            return 1;
        }
        else if (s1 >= 0 && s2 <= 0) {
            return 2;
        }
        else if (s1 <= 0 && s2 >= 0) {
            return 0;
        }
        else {
            return 3;
        }
    }


    tnl::Vector3 GetNearestPointAABB(const tnl::Vector3& point, const tnl::Vector3& aabb_max, const tnl::Vector3& aabb_min) {
        tnl::Vector3 q;
        float v = point.x;
        if (v < aabb_min.x) v = aabb_min.x;
        if (v > aabb_max.x) v = aabb_max.x;
        q.x = v;

        v = point.y;
        if (v < aabb_min.y) v = aabb_min.y;
        if (v > aabb_max.y) v = aabb_max.y;
        q.y = v;

        v = point.z;
        if (v < aabb_min.z) v = aabb_min.z;
        if (v > aabb_max.z) v = aabb_max.z;
        q.z = v;

        return q;
    }


	tnl::Vector3 GetNearestRectPoint(const tnl::Vector3& rect_pos, float w, float h, const tnl::Vector3 &point) {
		tnl::Vector3 nearest(0, 0, 0);
		float hw = w * 0.5f;
		float hh = h * 0.5f;
		float l = rect_pos.x - hw;
		float r = rect_pos.x + hw;
		float _stage_id = rect_pos.y - hh;
		float b = rect_pos.y + hh;
		nearest.x = (point.x > l && point.x < r) ? point.x : (point.x > l) ? r : l;
		nearest.y = (point.y > _stage_id && point.y < b) ? point.y : (point.y > _stage_id) ? b : _stage_id;
		return nearest;
	}


    tnl::Vector3 GetNearestRectPoint(const tnl::Vector3& rect_pos, const tnl::Vector3& axis_x, const tnl::Vector3& axis_y, const tnl::Vector3& rect_side_length, const tnl::Vector3& point) {
        tnl::Vector3 d = point - rect_pos;
        tnl::Vector3 q = rect_pos;
        float dist = tnl::Vector3::Dot(d, axis_x);
        if (dist > rect_side_length.x) dist = rect_side_length.x;
        if (dist < -rect_side_length.x) dist = -rect_side_length.x;
        q += axis_x * dist;

        dist = tnl::Vector3::Dot(d, axis_y);
        if (dist > rect_side_length.y) dist = rect_side_length.y;
        if (dist < -rect_side_length.y) dist = -rect_side_length.y;
        q += axis_y * dist;
        return q;
    }


    tnl::Vector3 GetNearestPointTriangle(const tnl::Vector3& p, const tnl::Vector3& a, const tnl::Vector3& b, const tnl::Vector3& c) {
        tnl::Vector3 ab = b - a;
        tnl::Vector3 ac = c - a;
        tnl::Vector3 ap = p - a;
        float d1 = tnl::Vector3::Dot(ab, ap);
        float d2 = tnl::Vector3::Dot(ac, ap);
        if (d1 <= 0.0f && d2 <= 0.0f) return a;

        tnl::Vector3 bp = p - b;
        float d3 = tnl::Vector3::Dot(ab, bp);
        float d4 = tnl::Vector3::Dot(ac, bp);
        if (d3 >= 0.0f && d4 <= d3) return b;

        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            float v = d1 / (d1 - d3);
            return a + ( ab * v );
        }

        tnl::Vector3 cp = p - c;
        float d5 = tnl::Vector3::Dot(ab, cp);
        float d6 = tnl::Vector3::Dot(ac, cp);
        if (d6 >= 0.0f && d5 <= d6) return c;
        
        float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            float w = d2 / (d2 - d6);
            return a + (ac * w);
        }

        float va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + ((c - b) * w);
        }

        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + (ab * v) + (ac * w);
    }


    tnl::Vector3 GetNearestPointPlane(const tnl::Vector3& v, const tnl::Vector3& pn, const tnl::Vector3& pv) {
        float d = tnl::Vector3::Dot(pn, pv);
        float _stage_id = tnl::Vector3::Dot(pn, v) - d;
        return v - (pn * _stage_id);
    }


    tnl::Vector3 GetNearestPointLine(const tnl::Vector3& v, const tnl::Vector3& a, const tnl::Vector3& b) {
        tnl::Vector3 ab = b - a;
        float _stage_id = tnl::Vector3::Dot( v-a, ab );
        if (_stage_id <= 0.0f) {
            return a;
        }
        else {
            float denom = tnl::Vector3::Dot(ab, ab);
            if (_stage_id >= denom) {
                return b;
            }
            else {
                _stage_id /= denom;
                return a + (ab * _stage_id);
            }
        }
    }


    std::tuple<tnl::Vector3, tnl::Vector3> GetNearestLines(const tnl::Vector3& s1, const tnl::Vector3& e1, const tnl::Vector3& s2, const tnl::Vector3& e2) {

        float s, _stage_id ;
        tnl::Vector3 c1, c2;

        tnl::Vector3 d1 = e1 - s1;
        tnl::Vector3 d2 = e2 - s2;
        tnl::Vector3 r = s1 - s2;
        float a = tnl::Vector3::Dot(d1, d1);
        float e = tnl::Vector3::Dot(d2, d2);
        float f = tnl::Vector3::Dot(d2, r);

        if (a <= FLT_EPSILON && e <= FLT_EPSILON) {
            s = _stage_id = 0.0f;
            c1 = s1;
            c2 = s2;
            return std::make_tuple(c1, c2);
        }
        if (a <= FLT_EPSILON) {
            s = 0.0f;
            _stage_id = f / e;
            _stage_id = std::clamp(_stage_id, 0.0f, 1.0f);
        }
        else {
            float c = tnl::Vector3::Dot(d1, r);
            if (e <= FLT_EPSILON) {
                _stage_id = 0.0f;
                s = std::clamp(-c/a, 0.0f, 1.0f);
            }
            else {
                float b = tnl::Vector3::Dot(d1, d2);
                float denom = a * e - b * b;
                if (denom != 0.0f) {
                    s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
                }
                else s = 0.0f;

                _stage_id = (b * s + f) / e;

                if (_stage_id < 0.0f) {
                    _stage_id = 0.0f;
                    s = std::clamp(-c/a, 0.0f, 1.0f);
                }
                else if(_stage_id > 1.0f) {
                    _stage_id = 1.0f;
                    s = std::clamp((b-c)/a, 0.0f, 1.0f);
                }
            }
        }
        c1 = s1 + d1 * s;
        c2 = s2 + d2 * _stage_id;
        return std::make_tuple( c1, c2 );

    }


    std::tuple<tnl::Vector3, float> CircumscribedCircle(const tnl::Vector3& a, const tnl::Vector3& b, const tnl::Vector3& c){
        tnl::Vector3 a2 = a * a;
        tnl::Vector3 b2 = b * b;
        tnl::Vector3 c2 = c * c;

        float CT = 2 * ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
        float x = ((c.y - a.y) * (b2.x - a2.x + b2.y - a2.y) + (a.y - b.y) * (c2.x - a2.x + c2.y - a2.y)) / CT;
        float y = ((a.x - c.x) * (b2.x - a2.x + b2.y - a2.y) + (b.x - a.x) * (c2.x - a2.x + c2.y - a2.y)) / CT;

        float r = (tnl::Vector3(x, y, 0) - a).length();

        return std::make_tuple(tnl::Vector3(x, y, 0), r);
    }


    Vector3 BezierSpline(const Vector3& _a1, const Vector3& _a2, const Vector3& _a3, const Vector3& _a4, float _stage_id) {

        _stage_id = (1.0f < _stage_id) ? 1.0f : _stage_id;
        _stage_id = (0.0f > _stage_id) ? 0.0f : _stage_id;

        float wkT = (1.0f - _stage_id);
        float wkX, wkY, wkZ;
        wkX = wkY = wkZ = 0.0f;

        float T2 = (_stage_id * _stage_id);
        float T3 = (_stage_id * _stage_id * _stage_id);
        float wkT2 = (wkT * wkT);
        float wkT3 = (wkT * wkT * wkT);

        wkX = ((wkT3)*_a1.x);
        wkX = wkX + ((3 * (wkT2)) * (_stage_id * _a2.x));
        wkX = wkX + (((3 * wkT) * (T2)) * (_a3.x));
        wkX = wkX + ((T3)*_a4.x);

        wkY = ((wkT3)*_a1.y);
        wkY = wkY + ((3 * (wkT2)) * (_stage_id * _a2.y));
        wkY = wkY + (((3 * wkT) * (T2)) * (_a3.y));
        wkY = wkY + ((T3)*_a4.y);

        wkZ = ((wkT3)*_a1.z);

        wkZ = wkZ + ((3 * (wkT2)) * (_stage_id * _a2.z));
        wkZ = wkZ + (((3 * wkT) * (T2)) * (_a3.z));
        wkZ = wkZ + ((T3)*_a4.z);

        return Vector3( wkX, wkY, wkZ );
    }


    int GetCorrectPositionIntersectAABB(const tnl::Vector3& a_prev, const tnl::Vector3& a_size, const tnl::Vector3& b, const tnl::Vector3& b_size, tnl::Vector3& out, const float correct_space)
    {
        const int DOWN = 0;
        const int RIGHT = 1;
        const int UP = 2;
        const int LEFT = 3;
        const int FRONT = 4;
        const int BACK = 5;
        tnl::Vector3 aa = a_prev - b;
        tnl::Vector3 bb = { 0, 0, 0 };
        tnl::Vector3 a_max = tnl::ToMaxAABB(aa, a_size);
        tnl::Vector3 a_min = tnl::ToMinAABB(aa, a_size);
        tnl::Vector3 b_max = tnl::ToMaxAABB(bb, b_size);
        tnl::Vector3 b_min = tnl::ToMinAABB(bb, b_size);
        tnl::Vector3 near = tnl::GetNearestPointAABB(bb, a_max, a_min);
        int region = tnl::GetRegionPointAndAABB(near, b_max, b_min);

        switch (region) {
        case UP :
            out.y = b.y + (b_size.y / 2) + (a_size.y / 2) + correct_space;
            return UP;
        case DOWN:
            out.y = b.y - (b_size.y / 2) - (a_size.y / 2) - correct_space;
            return DOWN;
        case RIGHT:
            out.x = b.x + (b_size.x / 2) + (a_size.x / 2) + correct_space;
            return RIGHT;
        case LEFT:
            out.x = b.x - (b_size.x / 2) - (a_size.x / 2) - correct_space;
            return LEFT;
        case BACK:
            out.z = b.z + (b_size.z / 2) + (a_size.z / 2) + correct_space;
            return BACK;
        case FRONT:
            out.z = b.z - (b_size.z / 2) - (a_size.z / 2) - correct_space;
            return FRONT;
        }
        return 0;
    }




    CubicSpline::CubicSpline(const std::vector<tnl::Vector3>& v) {

        int n = static_cast<int>(v.size()) - 1;

        for (int i = 0; i <= n; ++i) {
            a_.emplace_back(v[i]);
        }

        for (int i = 0; i <= n; ++i) {
            if (i == 0) {
                c_.emplace_back(tnl::Vector3{ 0, 0, 0 });
            }
            else if (i == n) {
                c_.emplace_back(tnl::Vector3{ 0, 0, 0 });
            }
            else {
                c_.emplace_back(tnl::Vector3{
                    3.0f * (a_[i - 1].x - 2.0f * a_[i].x + a_[i + 1].x),
                    3.0f * (a_[i - 1].y - 2.0f * a_[i].y + a_[i + 1].y),
                    3.0f * (a_[i - 1].z - 2.0f * a_[i].z + a_[i + 1].z)
                    });
            }
        }

        for (int i = 0; i < n; ++i) {
            if (i == 0) {
                w_.emplace_back(tnl::Vector3{ 0, 0, 0 });
            }
            else {
                float x = 4.0f - w_[i - 1].x;
                float y = 4.0f - w_[i - 1].y;
                float z = 4.0f - w_[i - 1].z;
                c_[i].x = (c_[i].x - c_[i - 1].x) / x;
                c_[i].y = (c_[i].y - c_[i - 1].y) / y;
                c_[i].z = (c_[i].z - c_[i - 1].z) / z;
                w_.emplace_back(tnl::Vector3{ 1.0f / x, 1.0f / y, 1.0f / z });
            }
        }

        for (int i = (n - 1); i > 0; --i) {
            c_[i] = c_[i] - c_[i + 1] * w_[i];
        }

        for (int i = 0; i <= n; ++i) {
            if (i == n) {
                d_.emplace_back(tnl::Vector3{ 0,0,0 });
                b_.emplace_back(tnl::Vector3{ 0,0,0 });
            }
            else {
                d_.emplace_back((c_[i + 1] - c_[i]) / 3.0f);
                b_.emplace_back(a_[i + 1] - a_[i] - c_[i] - d_[i]);
            }
        }

    }


    PointsLerp::PointsLerp(const std::vector< tnl::Vector3 >& points) {
        points_.resize(points.size());
        section_lengths_.resize(points.size());
        std::copy(points.begin(), points.end(), points_.begin());

        for (size_t i = 1; i < points_.size(); ++i) {
            section_lengths_[i] = (points_[i - 1] - points_[i]).length();
            all_length_ += section_lengths_[i];
            section_lengths_[i] += section_lengths_[i - 1];
        }
    }
    float PointsLerp::getLengthRate(float _stage_id) const noexcept {
        float n = 0;
        _stage_id = modff(_stage_id, &n);
        _stage_id = (_stage_id < 0.0f) ? 1.0f + _stage_id : _stage_id;
        return all_length_ * _stage_id;
    }

    float PointsLerp::getLengthPoint(int pn) const noexcept {
        if (pn < 0 || pn >= (int)section_lengths_.size()) return 0;
        return section_lengths_[pn];
    }

    int PointsLerp::getPrevPoint(float _stage_id) const noexcept {
        float d = getLengthRate(_stage_id);
        float c = 0.0f;
        for (size_t i = 0; i < section_lengths_.size(); ++i) {
            if (d >= section_lengths_[i]) continue;
            return (int)(i - 1);
        }
        return 0;
    }
}