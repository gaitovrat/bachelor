//
// Created by Richard Zvonek on 21-Nov-18.
//
#ifndef _LINETRACER_H
#define _LINETRACER_H

#include <utility>
#include <list>
#include <vector>
#include <cstdint>

#include "Shared/Image.h"
#include "Shared/Region.h"

#define CAMERA_LINE_LENGTH 128u


namespace MCU {
/**
 * @brief Třída pro hledání čáry
 */
class LineTracer {
public:

	/**
	 * @brief Konstruktor třídy
	 * @param listSize Počet uchovaných dat v historii
	 */
	LineTracer(uint64_t listSize);

	/**
	 * @brief Přidání obrazu do fronty
	 * @param Shared::Image Obrazová data z kamery
	 * @param forceSearchRegions Vynucení hledání podle Regionů
	 */
	void addImage(const Shared::Image &image, bool forceSearchRegions = false);

	/**
	 * @brief Metoda pro získání vzdálenosti levé čáry
	 * @return Vzdálenost levé čáry
	 */
	unsigned int getLeft();


	/**
	 * @brief Metoda pro získání vzdálenosti pravé čáry
	 * @return Vzdálenost pravé čáry
	 */
	unsigned int getRight();

	/**
	 * @brief Metoda pro získání naposledy nalezeného regionu.
	 * @return
	 */
	Shared::Region getLastRawRegion();

	/**
	 * @brief Getter pro zjištění, jestli byla nalezena levá čára
	 * @return True, pokud byla nalezena levá čára.
	 */
	bool hasLeft();


	/**
	 * @brief Getter pro zjištění, jestli byla nalezena pravá čára
	 * @return True, pokud byla nalezena pravá čára.
	 */
	bool hasRight();

	//private:

	/**
	 * @brief Metoda pro výpočet vzdáleností čar
	 *
	 * @param Shared::Image Obrazová data z kamery
	 * @param hasPrevDistance True, pokud existují vzdálenosti z předchozí iterace
	 * @param forceSearchRegions Vynucení hledání podle Regionů
	 * @return Vzdálenosti čar
	 */
	Shared::Region getDistances(const Shared::Image &image, bool hasPrevDistance, bool forceSearchRegions = false);

	/**
	 * @brief Metoda pro hledání čar podle předchozích nalezených čar
	 * @param Shared::Image  Obrazová data z kamery
	 * @param foundShared::Region Nalezený Region
	 * @return True, pokud byl nalezen Region
	 */
	bool findByPreviousIndex(const Shared::Image &image, Shared::Region &foundRegion);

	/**
	 * @brief Metoda pro hledání čar pomocí Regionů
	 * @param Shared::Image Obrazová daat z kamery
	 * @param leftIndex Počáteční index, od kterého se mají hledat čáry
	 * @param rightIndex Poslední index, do kterého se mají hledat čáry
	 * @param saveToClass True, pokud se mají nalezené Regiony uložit do třídy
	 * @return Nalezené regiony
	 */
	std::vector<Shared::Region>
	getRegions(const Shared::Image &image, uint8_t leftIndex = 0, uint8_t rightIndex = CAMERA_LINE_LENGTH - 1,
			   bool saveToClass = true);

	/**
	 * @brief Nalezené Regiony
	 */
	std::vector<Shared::Region> currentRegions_;
	/**
	 * @brief Velikost historie čar.
	 */
	uint64_t listSize_;


	/**
	 * @brief Proměnná pro uložení informace, jestli byl Shared::Region dopočítán
	 */
	bool computedRegion_;


	/**
	 * @brief Proměnná pro uložení informace, jestli je levá čára na podobném místě, jako v předchozím snímku
	 */
	bool unchangedLeft_;


	/**
	 * @brief Proměnná pro uložení informace, jestli je pravá čára na podobném místě, jako v předchozím snímku
	 */
	bool unchangedRight_;


	/**
	 * @brief Proměnná pro uložení informace, jestli byla nalezena levá čára
	 */
	bool hasLeft_;

	/**
	 * @brief Proměnná pro uložení informace, jestli byla nalezena pravá čára
	 */
	bool hasRight_;

	/**
	 * @brief Počet nalezených černých oblastí
	 */
	uint8_t blackRegionsCount_;

	/**
	 * @brief Počet nalezených bílých oblastí
	 */
	uint8_t whiteRegionsCount_;

	/**
	 * @brief Metoda pro získání vzdáleností nalezeného Reginu od okraje
	 * @return Vzdálenosti nalezeného Reginu od okrajů.
	 */
	std::pair<uint8_t, uint8_t> getDistancesPair();

	/**
	 * @brief prvni prvek je levy, druhy je pravy
	 */
	std::pair<uint8_t, uint8_t> currentAverage_;

	/**
	 * @brief prvni prvek je levy, druhy je pravy
	 */
	std::pair<uint8_t, uint8_t> currentMedian_;

	/**
	 * @brief Funkce pro reset hledání čar
	 */
	void reset();

	/**
	 * @brief Getter pro velikost historie čar
	 * @return
	 */
	uint32_t getListSize();

	/**
	 * @brief Fronta pro uložení čar.
	 */
	std::list<Shared::Region> imageRegionList_;

};
}

#endif //NXPCUPRASPI_LINETRACER_H
