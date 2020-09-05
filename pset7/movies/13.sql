SELECT name FROM people
WHERE id IN
(
	SELECT person_id FROM stars
	WHERE movie_id IN
	(
		SELECT stars.movie_id FROM 
			stars JOIN people
				ON stars.person_id = people.id
		WHERE 
			people.name = "Kevin Bacon" AND people.birth = 1958
	)
	
	EXCEPT
	
	SELECT id FROM people 
	WHERE 
		name = "Kevin Bacon" AND birth = 1958
);