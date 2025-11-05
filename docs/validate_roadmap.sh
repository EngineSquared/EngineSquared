#!/usr/bin/env bash

# ============================================================================
# Physics Plugin Roadmap - Validation Script
# ============================================================================
# Ce script valide l'intégrité complète des 26 issues du roadmap
# ============================================================================

set -euo pipefail

ISSUES_DIR="$(dirname "$0")/issues"
TOTAL_ISSUES=26
ERRORS=0

echo "🔍 EngineSquared Physics Plugin - Validation Script"
echo "════════════════════════════════════════════════════"
echo ""

# ============================================================================
# 1. Vérification de l'existence des fichiers
# ============================================================================
echo "📁 [1/5] Vérification de l'existence des fichiers..."

actual_count=$(ls "$ISSUES_DIR"/*.md 2>/dev/null | wc -l | tr -d ' ')
if [[ $actual_count -ne $TOTAL_ISSUES ]]; then
    echo "❌ ERROR: Expected $TOTAL_ISSUES issues, found $actual_count"
    ((ERRORS++))
else
    echo "✅ All 26 issue files present"
fi

echo ""

# ============================================================================
# 2. Vérification des dépendances
# ============================================================================
echo "🔗 [2/5] Vérification des dépendances..."

# Liste des issues existantes
existing_issues=$(ls "$ISSUES_DIR"/*.md | xargs -n1 basename | grep -oE "^[0-9]{3}")

# Extraire toutes les dépendances uniques
dependencies=$(grep -h "^\*\*Dependencies:\*\*" "$ISSUES_DIR"/*.md | grep -oE "#[0-9]{3}" | sed 's/#//' | sort -u)

dep_errors=0
for dep in $dependencies; do
    if ! echo "$existing_issues" | grep -q "^$dep$"; then
        echo "❌ ERROR: Dependency #$dep referenced but file does not exist"
        ((dep_errors++))
    fi
done

if [[ $dep_errors -eq 0 ]]; then
    echo "✅ All $(echo "$dependencies" | wc -l | tr -d ' ') dependency references are valid"
else
    echo "❌ Found $dep_errors invalid dependency references"
    ((ERRORS += dep_errors))
fi

echo ""

# ============================================================================
# 3. Vérification des Related Issues
# ============================================================================
echo "🔄 [3/5] Vérification des Related Issues..."

related_issues=$(grep -h "^\*\*Related Issues:\*\*" "$ISSUES_DIR"/*.md | grep -oE "#[0-9]{3}" | sed 's/#//' | sort -u)

related_errors=0
for rel in $related_issues; do
    if ! echo "$existing_issues" | grep -q "^$rel$"; then
        echo "❌ ERROR: Related issue #$rel referenced but file does not exist"
        ((related_errors++))
    fi
done

if [[ $related_errors -eq 0 ]]; then
    echo "✅ All $(echo "$related_issues" | wc -l | tr -d ' ') related issue references are valid"
else
    echo "❌ Found $related_errors invalid related issue references"
    ((ERRORS += related_errors))
fi

echo ""

# ============================================================================
# 4. Vérification des Follow-up Issues
# ============================================================================
echo "➡️  [4/5] Vérification des Follow-up Issues..."

followup_issues=$(grep -h "^\*\*Follow-up Issues:\*\*" "$ISSUES_DIR"/*.md | grep -oE "#[0-9]{3}" | sed 's/#//' | sort -u)

followup_errors=0
for followup in $followup_issues; do
    if ! echo "$existing_issues" | grep -q "^$followup$"; then
        echo "❌ ERROR: Follow-up issue #$followup referenced but file does not exist"
        ((followup_errors++))
    fi
done

if [[ $followup_errors -eq 0 ]]; then
    echo "✅ All $(echo "$followup_issues" | wc -l | tr -d ' ') follow-up issue references are valid"
else
    echo "❌ Found $followup_errors invalid follow-up issue references"
    ((ERRORS += followup_errors))
fi

echo ""

# ============================================================================
# 5. Vérification des milestones
# ============================================================================
echo "🎯 [5/5] Vérification de la distribution des milestones..."

v03_count=$(grep -c "^\*\*Milestone:\*\* v0.3" "$ISSUES_DIR"/*.md || echo 0)
v04_count=$(grep -c "^\*\*Milestone:\*\* v0.4" "$ISSUES_DIR"/*.md || echo 0)
v05_count=$(grep -c "^\*\*Milestone:\*\* v0.5" "$ISSUES_DIR"/*.md || echo 0)
v06_count=$(grep -c "^\*\*Milestone:\*\* v0.6" "$ISSUES_DIR"/*.md || echo 0)
v07_count=$(grep -c "^\*\*Milestone:\*\* v0.7" "$ISSUES_DIR"/*.md || echo 0)
v08_count=$(grep -c "^\*\*Milestone:\*\* v0.8" "$ISSUES_DIR"/*.md || echo 0)

echo "  v0.3: $v03_count issues (Core Completeness)"
echo "  v0.4: $v04_count issues (Collider Expansion)"
echo "  v0.5: $v05_count issues (Collision Queries)"
echo "  v0.6: $v06_count issues (Events & Constraints)"
echo "  v0.7: $v07_count issues (Advanced Physics)"
echo "  v0.8: $v08_count issues (Ecosystem & Tools)"

total_milestone_issues=$((v03_count + v04_count + v05_count + v06_count + v07_count + v08_count))
if [[ $total_milestone_issues -ne $TOTAL_ISSUES ]]; then
    echo "⚠️  WARNING: Milestone total ($total_milestone_issues) doesn't match expected ($TOTAL_ISSUES)"
fi

echo "✅ Milestone distribution checked"
echo ""

# ============================================================================
# Résumé Final
# ============================================================================
echo "════════════════════════════════════════════════════"
echo "📊 Résumé de la Validation"
echo "════════════════════════════════════════════════════"
echo "Total Issues: $TOTAL_ISSUES"
echo "Milestones: 6 (v0.3 → v0.8)"
echo "Dependencies Checked: $(echo "$dependencies" | wc -l | tr -d ' ')"
echo "Related Issues Checked: $(echo "$related_issues" | wc -l | tr -d ' ')"
echo "Follow-ups Checked: $(echo "$followup_issues" | wc -l | tr -d ' ')"
echo "Errors Found: $ERRORS"

if [[ $ERRORS -eq 0 ]]; then
    echo ""
    echo "✅ ✅ ✅  VALIDATION RÉUSSIE  ✅ ✅ ✅"
    echo ""
    echo "Le roadmap est complet et prêt pour l'implémentation !"
    echo ""
    echo "Prochaines étapes recommandées:"
    echo "  1. Créer les milestones sur GitHub"
    echo "  2. Créer les issues: gh issue create --body-file <file>"
    echo "  3. Configurer GitHub Projects pour le tracking"
    echo ""
    exit 0
else
    echo ""
    echo "❌ ❌ ❌  VALIDATION ÉCHOUÉE  ❌ ❌ ❌"
    echo ""
    echo "Veuillez corriger les $ERRORS erreur(s) détectée(s) avant de continuer."
    echo ""
    exit 1
fi
