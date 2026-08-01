import { useCallback, useEffect, useState, type FormEvent } from 'react'
import './App.css'

interface SynonymResult {
  word: string
  similarity: number
}

interface SynonymResponse {
  query: string
  method: string
  results: SynonymResult[]
}

type Method = 'graph' | 'brute'

// Picked from the actual loaded vocabulary (backend/data/vocabulary.txt) so
// these always resolve to a real, indexed word regardless of API quota.
const EXAMPLE_WORDS = ['happy', 'strong', 'smart', 'quick', 'simple', 'modern']

function App() {
  const [query, setQuery] = useState('')
  const [results, setResults] = useState<SynonymResult[]>([])
  const [method, setMethod] = useState<Method>('graph')
  const [k, setK] = useState(10)
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState<string | null>(null)
  const [lastQuery, setLastQuery] = useState<string | null>(null)
  const [wordCount, setWordCount] = useState<number | null>(null)
  const [elapsedMs, setElapsedMs] = useState<number | null>(null)

  useEffect(() => {
    fetch('/api/stats')
      .then((r) => r.json())
      .then((d) => setWordCount(d.wordCount))
      .catch(() => {
        /* stats are a nice-to-have, not fetching them shouldn't block the page */
      })
  }, [])

  const search = useCallback(
    async (word: string) => {
      const trimmed = word.trim()
      if (!trimmed) return

      setLoading(true)
      setError(null)
      const start = performance.now()

      try {
        const res = await fetch(
          `/api/synonyms?word=${encodeURIComponent(trimmed)}&k=${k}&method=${method}`,
        )
        if (!res.ok) {
          const body = await res.json().catch(() => ({}) as { error?: string })
          throw new Error(body.error || `Request failed (${res.status})`)
        }
        const data: SynonymResponse = await res.json()
        setResults(data.results.filter((r) => r.word.toLowerCase() !== trimmed.toLowerCase()))
        setLastQuery(trimmed)
        setElapsedMs(performance.now() - start)
      } catch (e) {
        setError(e instanceof Error ? e.message : 'Something went wrong')
        setResults([])
      } finally {
        setLoading(false)
      }
    },
    [k, method],
  )

  const handleSubmit = (e: FormEvent) => {
    e.preventDefault()
    search(query)
  }

  const handleExampleClick = (word: string) => {
    setQuery(word)
    search(word)
  }

  const handleResultClick = (word: string) => {
    setQuery(word)
    search(word)
  }

  return (
    <div className="page">
      <header className="hero">
        <h1>
          VectorDB <span className="accent">Synonyms</span>
        </h1>
        <p className="subtitle">
          Semantic synonym search over a vector database built from scratch in C++ — cosine
          similarity across live Gemini embeddings, no synonym lookup table involved.
        </p>
      </header>

      <form className="search-bar" onSubmit={handleSubmit}>
        <input
          type="text"
          value={query}
          onChange={(e) => setQuery(e.target.value)}
          placeholder="Type a word… e.g. happy"
          autoFocus
          spellCheck={false}
        />
        <button type="submit" disabled={loading || !query.trim()}>
          {loading ? 'Searching…' : 'Search'}
        </button>
      </form>

      {!lastQuery && (
        <div className="examples">
          <span className="examples-label">Try:</span>
          {EXAMPLE_WORDS.map((w) => (
            <button key={w} className="chip" onClick={() => handleExampleClick(w)}>
              {w}
            </button>
          ))}
        </div>
      )}

      <div className="controls">
        <label>
          Method
          <select value={method} onChange={(e) => setMethod(e.target.value as Method)}>
            <option value="graph">GraphANN (approximate)</option>
            <option value="brute">Brute force (exact)</option>
          </select>
        </label>
        <label>
          Results
          <select value={k} onChange={(e) => setK(Number(e.target.value))}>
            {[5, 10, 15, 20].map((n) => (
              <option key={n} value={n}>
                {n}
              </option>
            ))}
          </select>
        </label>
      </div>

      {error && <div className="error">⚠ {error}</div>}

      {lastQuery && !error && !loading && (
        <div className="results-meta">
          {results.length} result{results.length === 1 ? '' : 's'} for &ldquo;{lastQuery}&rdquo;
          {elapsedMs !== null && <span className="timing"> · {Math.round(elapsedMs)} ms</span>}
        </div>
      )}

      <ul className="results">
        {results.map((r, i) => (
          <li key={r.word} className="result-card" style={{ animationDelay: `${i * 30}ms` }}>
            <button className="result-word" onClick={() => handleResultClick(r.word)}>
              {r.word}
            </button>
            <div className="similarity-bar">
              <div
                className="similarity-fill"
                style={{ width: `${Math.max(0, Math.min(1, r.similarity)) * 100}%` }}
              />
            </div>
            <span className="similarity-value">{(r.similarity * 100).toFixed(1)}%</span>
          </li>
        ))}
      </ul>

      <footer className="footer">
        {wordCount !== null && <span>{wordCount.toLocaleString()} words indexed</span>}
      </footer>
    </div>
  )
}

export default App
