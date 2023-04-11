#include <libtarpe2d/collisions/find_contact_points.h>
#include <libtarpe2d/collisions/solve_collision.h>


void solve_collision(struct manifold * manifold)
{
	find_contact_points(manifold);

	struct rigidbody *body_1 = &(manifold->body_1->rb), *body_2 = &(manifold->body_2->rb);
	struct vec2 r1[2], r2[2];
	struct vec2 r1_normal, r2_normal;
	struct vec2 linear_angvel_1, linear_angvel_2;
	struct vec2 full_velocity_1, full_velocity_2;
	struct vec2 relative_velocity;
	float_t relvel_proj;
	float_t invMOI_coef_1, invMOI_coef_2;
	float_t j;

	struct vec2 normal_impulse[2], tangent_impulse[2] __attribute__((unused));

	// bool is_set[2] = {false, false};

	struct vec2 * contact;
	for (int i = 0; i < manifold->contact_points_count; ++i)
	{
		contact = manifold->contact_points + i;

		vec2_sub(contact, &(body_1->pos), r1 + i);
		vec2_sub(contact, &(body_2->pos), r2 + i);

		r1_normal = (struct vec2){.x = -r1[i].y, .y = r1[i].x};
		r2_normal = (struct vec2){.x = -r2[i].y, .y = r2[i].x};

		vec2_mul(&r1_normal, body_1->angular_velocity, &linear_angvel_1);
		vec2_mul(&r2_normal, body_2->angular_velocity, &linear_angvel_2);

		vec2_add(&(body_1->linear_velocity), &linear_angvel_1, &full_velocity_1);
		vec2_add(&(body_2->linear_velocity), &linear_angvel_2, &full_velocity_2);

		vec2_sub(&full_velocity_2, &full_velocity_1, &relative_velocity);

		relvel_proj = vec2_dot(&relative_velocity, &(manifold->normal));
		// if (relvel_proj > 0) continue;

		invMOI_coef_1 = flt_squared(vec2_dot(&r1_normal, &(manifold->normal)));
		invMOI_coef_2 = flt_squared(vec2_dot(&r2_normal, &(manifold->normal)));

		j = (-2 * relvel_proj)
		    / (body_1->inv_mass + body_2->inv_mass
		       + invMOI_coef_1 * body_1->inv_moment_of_inertia
		       + invMOI_coef_2 * body_2->inv_moment_of_inertia);
		j /= manifold->contact_points_count;

		vec2_mul(&(manifold->normal), j, normal_impulse + i);
		// is_set[i] = true;
	}

	struct vec2 tmp, *cur_normal_impulse;
	for (int i = 0; i < manifold->contact_points_count; ++i)
	{
		// if (!is_set[i]) continue;
		cur_normal_impulse = normal_impulse + i;

		vec2_mul(cur_normal_impulse, -body_1->inv_mass, &tmp);
		vec2_add(&(body_1->linear_velocity), &tmp, &(body_1->linear_velocity));
		body_1->angular_velocity +=
			-vec2_cross_abs(r1 + i, cur_normal_impulse) * body_1->inv_moment_of_inertia;

		vec2_mul(cur_normal_impulse, body_2->inv_mass, &tmp);
		vec2_add(&(body_2->linear_velocity), &tmp, &(body_2->linear_velocity));
		body_2->angular_velocity +=
			vec2_cross_abs(r2 + i, cur_normal_impulse) * body_2->inv_moment_of_inertia;
		
		// vec2_nullify(&(body_1->tick_force));
		// vec2_nullify(&(body_2->tick_force));
	}
}
